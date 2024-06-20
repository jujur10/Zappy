#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cmath>

int main() {
    InitWindow(1280, 720, "Raylib Example");
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;

    SetTargetFPS(144);

    int cameraMode = CAMERA_FREE;

    Vector3 planePosition = { 0.0f, 0.0f, 0.0f };

    Vector2 selectedCell = { 0, 0 };

    float cellSize = 2.0f;

    int cellCountX = 15;
    int cellCountY = 5;

    float planeSizeX = cellSize * cellCountX / 2.0f;
    float planeSizeY = cellSize * cellCountY / 2.0f;

    Vector3 hitPosition = { 0.0f, 0.0f, 0.0f };
    bool hitDetected = false;

    bool step = false;

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, cameraMode);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            hitDetected = false;
            
            BoundingBox box = {
                { planePosition.x - planeSizeX, planePosition.y, planePosition.z - planeSizeY },
                { planePosition.x + planeSizeX, planePosition.y, planePosition.z + planeSizeY }
            };
            RayCollision collision = GetRayCollisionBox(ray, box);
            if (collision.hit) {
                hitDetected = true;
                hitPosition = collision.point;
            }
            
            // std::cout << "position: " << hitPosition.x << " " << hitPosition.y << " " << hitPosition.z << std::endl;
            
            step = ((int)std::round(hitPosition.z / cellSize) % 2) == 0;
            
            if (step && hitDetected) {
                hitPosition.x -= cellSize / 2;
            }

            selectedCell = (Vector2){ std::round(hitPosition.x / cellSize) * cellSize, std::round(hitPosition.z / cellSize) * cellSize };
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(camera);

        // Draw floor
        DrawPlane(planePosition, (Vector2){ planeSizeX * 2, planeSizeY * 2 }, GREEN);

        if (step)
            DrawPlane((Vector3){ selectedCell.x + cellSize / 2, 0.01f, selectedCell.y}, (Vector2){ cellSize, cellSize }, BLUE);
        else
            DrawPlane((Vector3){ selectedCell.x, 0.01f, selectedCell.y}, (Vector2){ cellSize, cellSize }, BLUE);

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
