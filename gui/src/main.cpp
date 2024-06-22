#include <cstring>
#include <thread>

#include "Camera3D.hpp"
#include "Window.hpp"
#include "ModelAnimation.hpp"
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
#include "player.hpp"
#include "raygui.h"
#include "style_bluish.h"
#include "gui.hpp"

namespace zappy_gui
{

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr char help[] = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth = 1'280;
constexpr int32_t screenHeight = 720;
constexpr uint32_t serverToGuiQueueCapacity = 4096;  // 4096 is the minimum size for the queues
constexpr uint32_t GuiToServerQueueCapacity = 4096;

net::STGQueue net::ServerToGuiQueue{serverToGuiQueueCapacity};
net::GTSQueue net::GuiToServerQueue{GuiToServerQueueCapacity};

}  // namespace zappy_gui

static void InitializeECS(flecs::world &ecs,
                          raylib::Camera3D const &camera,
                          raylib::Model &sand,
                          raylib::Model &sandRock,
                          raylib::Model &sandCactus,
                          raylib::Model &food,
                          raylib::Model &crystal,
                          raylib::Model &skybox,
                          std::vector<raylib::ModelAnimation> &playerAnimations,
                          raylib::Texture2D &inProgressIcon,
                          raylib::Texture2D &successIcon,
                          raylib::Texture2D &failureIcon,
                          raylib::Model &eggModel)
{
    ecs.set_entity_range(4'269'420, 0);  // Allow flecs to only generate entity ids starting from 4'269'420

    ecs.import <flecs::monitor>();
    ecs.import <flecs::metrics>();
    ecs.set<flecs::Rest>({});  // Start REST API with default parameters

    ecs.set<raylib::Camera3D>(camera);

    ecs.set<zappy_gui::map::tileModels>({&sand, &sandRock, &sandCactus});

    ecs.set<zappy_gui::map::resourceModels>({&food, &crystal});

    ecs.set<zappy_gui::player::PlayerAnimations>({&playerAnimations});

    ecs.set<zappy_gui::map::skybox>({&skybox});

    ecs.set<zappy_gui::player::IncantationIcons>({&inProgressIcon, &successIcon, &failureIcon});

    ecs.set<zappy_gui::player::EggModel>({&eggModel});

    zappy_gui::systems::registerSystems(ecs);

    ecs.progress();  // Progress through OnStart pipeline // NOLINT

    zappy_gui::gui::createGuiEntities(ecs, zappy_gui::screenWidth, zappy_gui::screenHeight);
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
    ::DisableCursor(); // Hides cursor and locks it to the window
    raygui::GuiLoadStyleBluish();
    raygui::GuiUnlock();

    //--------------------------------------------------------------------------------------
    // Create the 3D camera
    raylib::Camera3D camera({10.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.f, CAMERA_PERSPECTIVE);

    //--------------------------------------------------------------------------------------
    // Load the 3D models for tiles and resources
    auto sand = raylib::Model("gui/resources/assets/sand.glb");
    auto sandRock = raylib::Model("gui/resources/assets/sand_rocks.glb");
    auto sandCactus = raylib::Model("gui/resources/assets/sand_cactus.glb");
    auto food = raylib::Model("gui/resources/assets/food.glb");
    auto crystal = raylib::Model("gui/resources/assets/crystal.glb");
    auto skybox = raylib::Model("gui/resources/assets/skybox.glb");
    auto inProgressIcon = raylib::Texture2D("gui/resources/assets/hourglass.png");
    auto successIcon = raylib::Texture2D("gui/resources/assets/lvlup.png");
    auto failureIcon = raylib::Texture2D("gui/resources/assets/cross.png");
    std::vector<raylib::ModelAnimation> playerAnimations = raylib::ModelAnimation::Load("gui/resources/assets/cactoro.m3d");
    auto eggModel = raylib::Model("gui/resources/assets/cactoro_egg.m3d");

    //--------------------------------------------------------------------------------------
    // Create the ECS and initialize it
    flecs::world ecs;
    ::InitializeECS(ecs,
        camera,
        sand, sandRock, sandCactus,
        food, crystal,
        skybox,
        playerAnimations,
        inProgressIcon, successIcon, failureIcon,
        eggModel);

    //--------------------------------------------------------------------------------------
    // Start the network main loop on another thread
    std::jthread networkThread(zappy_gui::net::NetworkTreadLoop, serverSocket);

    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!window.ShouldClose() && ecs.progress())  // Detect window close button or ESC key
    {
        // Main loop
    }
    //--------------------------------------------------------------------------------------
    // Stop the network thread
    networkThread.request_stop();

    //--------------------------------------------------------------------------------------
    // Unload the shaders
    const auto *tileModels = ecs.get_mut<zappy_gui::map::tileModels>();
    zappy_gui::utils::UnloadShaders(tileModels->sand);
    zappy_gui::utils::UnloadShaders(tileModels->sandRock);
    zappy_gui::utils::UnloadShaders(tileModels->sandCactus);

    const auto *resourceModels = ecs.get_mut<zappy_gui::map::resourceModels>();
    zappy_gui::utils::UnloadShaders(resourceModels->food);
    zappy_gui::utils::UnloadShaders(resourceModels->crystal);
}
