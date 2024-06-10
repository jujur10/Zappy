#include "map.hpp"
#include "my_write.hpp"
#include "sockets.hpp"
#include "systems.hpp"

#include <Camera3D.hpp>
#include <Window.hpp>
#include <cstring>
#include <flecs.h>
#include <my_exit.hpp>

namespace zappy_gui
{

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr const char *const help = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth    = 1'280;
constexpr int32_t screenHeight   = 720;

}

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (2 == argc && 0 == std::strcmp(argv[1], "-help"))
    {
        const zappy_gui::FileWriter writer(2);
        writer.writeNoReturn(zappy_gui::help, sizeof(zappy_gui::help));
        return 0;
    }
    if (5 != argc)
    {
        zappy_gui::SystemExit::exit(1);
    }
    const auto serverSocket = zappy_gui::connectToServer(argv);

    flecs::world ecs;

    ecs.import <flecs::monitor>();
    ecs.import <flecs::metrics>();
    ecs.import <flecs::alerts>();
    // Start REST API with default parameters
    ecs.set<flecs::Rest>({});

    ::SetConfigFlags(FLAG_MSAA_4X_HINT);
    raylib::Window window(zappy_gui::screenWidth, zappy_gui::screenHeight, "raylib-cpp - basic window");
    window.SetTargetFPS(60);

    raylib::Camera3D camera(
        {10.0f, 10.0f, 10.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        45.f,
        CAMERA_PERSPECTIVE
        );
    ecs.set<raylib::Camera3D>(camera);

    DisableCursor(); // Hides cursor and locks it to the window

    auto innerMod = raylib::Model("gui/resources/assets/grass_top.glb");
    auto outerMod = raylib::Model("gui/resources/assets/grass_full.glb");
    ecs.set<zappy_gui::map::tileModels>({&innerMod, &outerMod});

    zappy_gui::systems::registerSystems(ecs);

    ecs.progress(); // Progress throught OnStart pipeline
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!window.ShouldClose() && ecs.progress()) // Detect window close button or ESC key
    {
    }

    const auto tileModels = ecs.get_mut<zappy_gui::map::tileModels>();
    ::UnloadShader(tileModels->outerModel->GetMaterials()[0].shader);
    ::UnloadShader(tileModels->innerModel->GetMaterials()[0].shader);

    return 0;
}
