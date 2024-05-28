#include "Window.hpp"
#include "sockets.hpp"
#include "my_write.hpp"

#include <cstring>
namespace zappy_gui
{
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr char const * const help = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth = 800;
constexpr int32_t screenHeight = 450;

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void updateDrawFrame()  // Update and Draw one frame
{
    // Update
    //----------------------------------------------------------------------------------
    // Update variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    ::BeginDrawing();

    ::ClearBackground(RAYWHITE);

    ::DrawText("raylib-cpp window!", 160, 200, 20, LIGHTGRAY);

    ::EndDrawing();
    //----------------------------------------------------------------------------------
}
}

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if ((2 == argc) && (0 == ::strcmp(argv[1], "-help")))
    {
        zappy_gui::FileWriter writer(2);
        writer.writeNoReturn(zappy_gui::help, sizeof(zappy_gui::help));
        return 0;
    }
    if (5 != argc)
    {
        ::_exit(1);
    }
    const auto serverSocket = zappy_gui::connectToServer(argv);
    raylib::Window window(zappy_gui::screenWidth, zappy_gui::screenHeight, "raylib-cpp - basic window");

    ::SetTargetFPS(60); // Set game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose())    // Detect window close button or ESC key
    {
        ::BeginDrawing();
        zappy_gui::updateDrawFrame();
    }
    return 0;
}
