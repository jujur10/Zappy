#include <cstring>
#include <thread>

#include "Camera3D.hpp"
#include "Window.hpp"
#include "flecs.h"
#include "gui_to_server_cmd_structs.hpp"
#include "map.hpp"
#include "my_exit.hpp"
#include "my_write.hpp"
#include "networking.hpp"
#include "raylib_utils.hpp"
#include "server_handshake.hpp"
#include "server_to_gui_cmd_structs.hpp"
#include "sockets.hpp"
#include "systems.hpp"

namespace zappy_gui
{

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr const char *const help = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth = 1'280;
constexpr int32_t screenHeight = 720;
constexpr uint32_t serverToGuiQueueCapacity = 4096;  // 4096 is the minimum size for the queues
constexpr uint32_t GuiToServerQueueCapacity = 4096;

net::STGQueue net::ServerToGuiQueue{serverToGuiQueueCapacity};
net::GTSQueue net::GuiToServerQueue{GuiToServerQueueCapacity};

}  // namespace zappy_gui

static void InitializeECS(flecs::world &ecs,
                          raylib::Camera3D const &camera,
                          raylib::Model &grass,
                          raylib::Model &food,
                          raylib::Model &crystal)
{
    ecs.set_entity_range(4'269'420, 0);  // Allow flecs to only generate entity ids starting from 4'269'420

    ecs.import <flecs::monitor>();
    ecs.import <flecs::metrics>();
    ecs.set<flecs::Rest>({});  // Start REST API with default parameters

    ecs.set<raylib::Camera3D>(camera);

    ecs.set<zappy_gui::map::tileModels>({&grass});

    ecs.set<zappy_gui::map::resourceModels>({&food, &crystal});

    zappy_gui::systems::registerSystems(ecs);

    ecs.progress();  // Progress through OnStart pipeline // NOLINT
}

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int32_t main(const int32_t argc, char *argv[])
{
    // Parse the command line arguments
    if (2 == argc && 0 == std::strcmp(argv[1], "-help"))
    {
        const zappy_gui::FileWriter writer(2);
        writer.writeNoReturn(zappy_gui::help, sizeof(zappy_gui::help));  // NOLINT
        return 0;
    }
    if (5 != argc)
    {
        zappy_gui::SystemExit::exit(1);
    }

    //--------------------------------------------------------------------------------------
    // Connect to the server and perform the handshake
    const zappy_gui::Socket serverSocket = zappy_gui::ConnectToServer(argv);
    Handshake(serverSocket);

    //--------------------------------------------------------------------------------------
    // Setup the window with anti-aliasing and a target of 60 FPS
    ::SetConfigFlags(FLAG_MSAA_4X_HINT);
    raylib::Window window(zappy_gui::screenWidth, zappy_gui::screenHeight, "raylib-cpp - basic window");
    window.SetTargetFPS(60);

    //--------------------------------------------------------------------------------------
    // Create the 3D camera
    raylib::Camera3D camera({10.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.f, CAMERA_PERSPECTIVE);

    //--------------------------------------------------------------------------------------
    // Load the 3D models for tiles and resources
    auto grassMod = raylib::Model("gui/resources/assets/grass2.glb");
    auto food = raylib::Model("gui/resources/assets/food.glb");
    auto crystal = raylib::Model("gui/resources/assets/crystal1_optimized.glb");

    //--------------------------------------------------------------------------------------
    // Create the ECS and initialize it
    flecs::world ecs;
    ::InitializeECS(ecs, camera, grassMod, food, crystal);

    //--------------------------------------------------------------------------------------
    // Start the network main loop on another thread
    std::jthread networkThread(zappy_gui::net::NetworkTreadLoop, serverSocket);

    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!window.ShouldClose() && ecs.progress())  // Detect window close button or ESC key
    {
    }

    //--------------------------------------------------------------------------------------
    // Stop the network thread
    networkThread.request_stop();

    //--------------------------------------------------------------------------------------
    // Unload the shaders
    const auto *tileModels = ecs.get_mut<zappy_gui::map::tileModels>();
    zappy_gui::utils::UnloadShaders(tileModels->grass);

    const auto *resourceModels = ecs.get_mut<zappy_gui::map::resourceModels>();
    zappy_gui::utils::UnloadShaders(resourceModels->food);
    zappy_gui::utils::UnloadShaders(resourceModels->crystal);
}
