#include "Window.hpp"
#include "sockets.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr int screenWidth = 800;
constexpr int screenHeight = 450;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame();     // Update and Draw one frame

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (argc != 5)
    {
        ::_exit(1);
    }
    const uint16_t server_fd = zappy_gui::connectToServer(argv);
    raylib::Window window(screenWidth, screenHeight, "raylib-cpp - basic window");

    SetTargetFPS(60);   // Set game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame()
{
    // Update
    //----------------------------------------------------------------------------------
    // Update variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("raylib-cpp window!", 160, 200, 20, LIGHTGRAY);

    EndDrawing();
    //----------------------------------------------------------------------------------
}