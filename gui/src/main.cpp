#include "Window.hpp"
#include "sockets.h"

#include <cstring>
namespace zappy_gui
{
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr size_t help_len = 39;
constexpr char help[help_len] = "USAGE: ./zappy_gui -p port -h machine\n";
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
        ::write(2, zappy_gui::help, zappy_gui::help_len);
        return 0;
    }
    if (5 != argc)
    {
        ::_exit(1);
    }
    const uint16_t server_fd = zappy_gui::connectToServer(argv);
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
