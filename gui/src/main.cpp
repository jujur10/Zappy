#include "Window.hpp"
#include "flecs.h"
#include "my_write.hpp"
#include "rlgl.h"
#include "sockets.hpp"

#include <cstring>

constexpr int MAP_WIDTH = 1024;
constexpr int MAP_HEIGHT = 1024;
constexpr float TILE_SIZE = 1.f;
constexpr float TILE_HEIGHT = 0.0f; // Adjust this value to change the height of the tiles
constexpr float VERTICAL_SPACING = (sqrt(3.0f) / 2.0f) * TILE_SIZE * 1.05;

using tileModelTop  = Model;
using tileModelFull = Model;

struct tileType
{
    Model* model;
    Matrix* transforms;
    uint64_t transformCount;
};

struct tileModel
{
    Model* innerModel;
    Model* outerModel;
};

template <typename T>
struct vector {
    size_t size;
    T *data;
};

struct tileTransforms
{
    vector<Matrix> inner;
    vector<Matrix> outer;
};

struct innerTile { };
struct outerTile { };

namespace zappy_gui
{

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
constexpr char const * const help = "USAGE: ./zappy_gui -p port -h machine\n";
constexpr int32_t screenWidth = 1280;
constexpr int32_t screenHeight = 720;

void DrawModelInstanced(const Model* model, const Matrix* matrixArray, uint64_t count)
{
    for (int i = 0; i < model->meshCount; i++)
    {
        DrawMeshInstanced(model->meshes[i], model->materials[model->meshMaterial[i]], matrixArray, count);
    }
}

void SetupModel(Model* model)
{
    Shader shader = LoadShader("gui/resources/shaders/tile_instancing.vs", NULL);
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    for (int i = 0; i < model->materialCount; i++)
        model->materials[i].shader = shader;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void updateDrawFrame(Camera &camera, const Model &modelFull, const Model &modelTop, const int MAP_WIDTH, const int MAP_HEIGHT, Matrix *borderTransforms, Matrix *innerTransforms)  // Update and Draw one frame
{
    // Update
    //----------------------------------------------------------------------------------
    // Update variables here
    //----------------------------------------------------------------------------------
    UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
                0.0f                                                // Move up-down
            },
            (Vector3){
                GetMouseDelta().x*0.05f,                            // Rotation: yaw
                GetMouseDelta().y*0.05f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove()*2.0f);                              // Move to target (zoom)
    // Draw
    //----------------------------------------------------------------------------------
    ::BeginDrawing();

    ::ClearBackground(BLACK);

    BeginMode3D(camera);
    rlDisableBackfaceCulling();

    // Draw border tiles using the complete model
    DrawModelInstanced(&modelFull, borderTransforms, MAP_WIDTH * 2 + (MAP_HEIGHT - 2) * 2);

    // Draw inner tiles using the model with only the top face
    DrawModelInstanced(&modelTop, innerTransforms, (MAP_WIDTH - 2) * (MAP_HEIGHT - 2));

    DrawGrid(10, 1.0f);         // Draw a grid

    EndMode3D();


    DrawFPS(10, 10);

    ::EndDrawing();
    //----------------------------------------------------------------------------------
}
}

void drawTiles(flecs::iter& it, tileModel *models)
{
    const auto innerTiles = it.world().get<tileTransforms>()->inner;
    const auto outerTiles = it.world().get<tileTransforms>()->outer;

    zappy_gui::DrawModelInstanced(models->innerModel, innerTiles.data, innerTiles.size + 1);
    zappy_gui::DrawModelInstanced(models->outerModel, outerTiles.data, outerTiles.size + 1);
}

void registerTileDrawing(flecs::world &ecs)
{
    // Create a system for moving an entity
    // ecs.system<const Vector3>()
    //     .kind(flecs::OnUpdate) // A phase orders a system in a pipeline
    //     .each([](const Vector3 position) {
    //     });
    ecs.system<Camera>("updateCamera")
    .term_at(1).singleton()
    .kind(flecs::OnUpdate)
    .iter([]([[maybe_unused]] const flecs::iter &it, Camera *camera) {
        UpdateCameraPro(camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
                0.0f                                                // Move up-down
            },
            (Vector3){
                GetMouseDelta().x*0.05f,                            // Rotation: yaw
                GetMouseDelta().y*0.05f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove()*2.0f);                              // Move to target (zoom)
    });

    ecs.system<Camera>("setupDrawing")
    .term_at(1).singleton()
    .kind(flecs::PreUpdate)
    .iter([]([[maybe_unused]] const flecs::iter &it, const Camera * const camera) {
        ::BeginDrawing();

        ::ClearBackground(BLACK);

        BeginMode3D(*camera);
        rlDisableBackfaceCulling();
    });

    ecs.system<Matrix, outerTile>("transformOuterTile")
    .kind(flecs::PreUpdate)
    .each([](flecs::iter& it, size_t index, Matrix &position, outerTile type) {
        auto outer = it.world().get_mut<tileTransforms>();
        outer->outer.data[outer->outer.size] = position;
        outer->outer.size++;
    });

    ecs.system<Matrix, innerTile>("transformInnerTile")
    .kind(flecs::PreUpdate)
    .iter([](flecs::iter& it, Matrix *position, innerTile *type) {
        auto inner = it.world().get_mut<tileTransforms>();
        std::memcpy(inner->inner.data, position, it.count() * sizeof(Matrix));
        inner->inner.size += it.count() - 1;
        // auto innerModel = it.world().get<tileModel>()->innerModel;
        // zappy_gui::DrawModelInstanced(innerModel, position, it.count());
    });

    ecs.system<tileModel>("drawTiles")
    .term_at(1).singleton()
    .kind(flecs::OnUpdate)
    .iter(drawTiles);

    ecs.system("endDrawing")
    .kind(flecs::PostUpdate)
    .iter([]([[maybe_unused]] const flecs::iter &it) {
        EndMode3D();

        DrawFPS(10, 10);
        ::EndDrawing();

        auto outer = it.world().get_mut<tileTransforms>();
        // memset(outer->outer.data, 0, outer->outer.size + 1);
        outer->outer.size = 0;

        auto inner = it.world().get_mut<tileTransforms>();
        // memset(inner->inner.data, 0, inner->inner.size + 1);
        inner->inner.size = 0;
    });

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
    // const auto serverSocket = zappy_gui::connectToServer(argv);


    flecs::world ecs;

    ecs.import<flecs::monitor>();
    ecs.import<flecs::metrics>();
    ecs.import<flecs::alerts>();
    // Start REST API with default parameters
    ecs.set<flecs::Rest>({});





    SetConfigFlags(FLAG_MSAA_4X_HINT);
    raylib::Window window(zappy_gui::screenWidth, zappy_gui::screenHeight, "raylib-cpp - basic window");

    Camera camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


    tileModelFull modelFull = LoadModel("gui/resources/assets/grass_full.glb");
    tileModelTop modelTop = LoadModel("gui/resources/assets/grass_top.glb");
    zappy_gui::SetupModel(&modelFull);
    zappy_gui::SetupModel(&modelTop);

    ecs.set<tileModel>({&modelTop, &modelFull});

    // Matrix *borderTransforms = new Matrix[MAP_WIDTH * 2 + (MAP_HEIGHT - 2) * 2];
    // Matrix *innerTransforms  = new Matrix[(MAP_WIDTH - 2) * (MAP_HEIGHT - 2)];
    int borderIndex = 0;
    int innerIndex = 0;
    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            float offsetX = (y % 2 == 0) ? 0.0f : TILE_SIZE * 0.5f;
            Vector3 position = {
                ((x * TILE_SIZE + offsetX) - (MAP_WIDTH * TILE_SIZE * 0.5f)) * 1.05,
                TILE_HEIGHT,
                y * TILE_SIZE * VERTICAL_SPACING - (MAP_HEIGHT * TILE_SIZE * 0.375f)
            };

            // Assign the transform to the appropriate array based on tile position
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1)
            {
                // borderTransforms[borderIndex] = MatrixTranslate(position.x, position.y, position.z);
                ecs.entity()
                .set<Matrix>(MatrixTranslate(position.x, position.y, position.z))
                .add<outerTile>();
                borderIndex++;
            }
            else
            {
                // innerTransforms[innerIndex] = MatrixTranslate(position.x, position.y, position.z);
                ecs.entity()
                .set<Matrix>(MatrixTranslate(position.x, position.y, position.z))
                .add<innerTile>();
                innerIndex++;
            }
        }
    }

    // Load the animated model mesh and basic data

    // Model modelFull = LoadModel("gui/resources/assets/grass_full.glb");
    // Model modelTop = LoadModel("gui/resources/assets/grass_top.glb");
    // zappy_gui::SetupModel(&modelFull);
    // zappy_gui::SetupModel(&modelTop);


    // ecs.entity()
    //     .set<tileType>({&modelFull, borderTransforms, (MAP_WIDTH * 2 + (MAP_HEIGHT - 2) * 2)});
    //
    // ecs.entity()
    //     .set<tileType>({&modelTop, innerTransforms, ((MAP_WIDTH - 2) * (MAP_HEIGHT - 2))});

    tileTransforms tile_transforms;
    tile_transforms.inner.size = 0;
    tile_transforms.inner.data = new Matrix[(MAP_WIDTH - 2) * (MAP_HEIGHT - 2)];
    tile_transforms.outer.size = 0;
    tile_transforms.outer.data = new Matrix[MAP_WIDTH * 2 + (MAP_HEIGHT - 2) * 2];
    ecs.set<tileTransforms>(tile_transforms);

    // ecs.entity()
    //     .set<tileType>({&modelTop, innerTransforms, ((MAP_WIDTH - 2) * (MAP_HEIGHT - 2))});

    ecs.set<Camera>(camera);

    registerTileDrawing(ecs);
    //::SetTargetFPS(60); // Set game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose() && ecs.progress())    // Detect window close button or ESC key
    {
        // zappy_gui::updateDrawFrame(camera, modelFull, modelTop, MAP_WIDTH, MAP_HEIGHT, borderTransforms, innerTransforms);
    }
    // delete[] borderTransforms;
    // delete[] innerTransforms;
    UnloadShader(modelFull.materials[0].shader);
    UnloadShader(modelTop.materials[0].shader);
    UnloadModel(modelFull);
    UnloadModel(modelTop);
    delete[] tile_transforms.inner.data;
    delete[] tile_transforms.outer.data;
    return 0;
}
