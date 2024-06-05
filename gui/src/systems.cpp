//
// Created by quentinf on 05/06/24.
//

#include "map.hpp"
#include "systems.hpp"

#include <Camera3D.hpp>
#include <flecs.h>
#include <rlgl.h>

namespace zappy_gui::systems
{
static void setupModel(const raylib::Model *const model, const char *const vertShaderPath, const char *const fragShaderPath)
{
    Shader shader                        = ::LoadShader(vertShaderPath, fragShaderPath);
    shader.locs[SHADER_LOC_MATRIX_MODEL] = ::GetShaderLocationAttrib(shader, "instanceTransform");
    shader.locs[SHADER_LOC_MATRIX_MVP]   = ::GetShaderLocation(shader, "mvp");
    for (int32_t i = 0; i < model->materialCount; ++i)
    {
        model->materials[i].shader = shader;
    }
}

static void drawModelInstanced(const raylib::Model *const model, const Matrix *const matrixArray, const int32_t count)
{
    for (int32_t i = 0; i < model->meshCount; ++i)
    {
        ::DrawMeshInstanced(model->meshes[i], model->materials[model->meshMaterial[i]], matrixArray, count);
    }
}

static void registerOnStartSystems(flecs::world &ecs)
{
    ecs.system("generateMap")
        .kind(flecs::OnStart)
        .iter(map::generateMap);

    ecs.system<map::tileModels>("loadInstancingShader")
        .kind(flecs::OnStart)
        .iter([](flecs::iter &it, const map::tileModels * const models) {
            setupModel(&models->outerModel, "gui/resources/shaders/tile_instancing.vs", nullptr);
            setupModel(&models->innerModel, "gui/resources/shaders/tile_instancing.vs", nullptr);
        });
}

static void registerPreUpdateSystems(flecs::world &ecs)
{
    ecs.system<raylib::Camera3D>("setupDrawing")
        .term_at(1).singleton()
        .kind(flecs::PreUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D * const camera) {
            ::BeginDrawing();

            ::ClearBackground(BLACK);

            camera->BeginMode();

            ::rlDisableBackfaceCulling();
        });
}

static void registerOnUpdateSystems(flecs::world &ecs)
{
    ecs.system<raylib::Camera3D>("updateCamera")
        .term_at(1).singleton()
        .kind(flecs::OnUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D * const camera) {
            camera->Update(
                {
                    (::IsKeyDown(KEY_W) || ::IsKeyDown(KEY_UP)) * 0.1f -    // Move forward-backward
                        (::IsKeyDown(KEY_S) || ::IsKeyDown(KEY_DOWN)) * 0.1f,
                    (::IsKeyDown(KEY_D) || ::IsKeyDown(KEY_RIGHT)) * 0.1f - // Move right-left
                        (::IsKeyDown(KEY_A) || ::IsKeyDown(KEY_LEFT)) * 0.1f,
                    0.0f                                                    // Move up-down
                },
                {
                    ::GetMouseDelta().x * 0.05f, // Rotation: yaw
                    ::GetMouseDelta().y * 0.05f, // Rotation: pitch
                    0.0f                       // Rotation: roll
                },
                ::GetMouseWheelMove() * 2.0f);   // Move to target (zoom)
        });

    ecs.system<Matrix, map::outerTile>("drawOuterTile")
        .kind(flecs::OnUpdate)
        .iter([](const flecs::iter &it, const Matrix *const tilesPosition, [[maybe_unused]] const map::outerTile *const type) {
            drawModelInstanced(&it.world().get<map::tileModels>()->outerModel, tilesPosition, static_cast<int32_t>(it.count()));
        });

    ecs.system<Matrix, map::innerTile>("drawInnerTile")
        .kind(flecs::OnUpdate)
        .iter([](const flecs::iter &it, const Matrix *const tilesPosition, [[maybe_unused]] const map::innerTile *const type) {
            drawModelInstanced(&it.world().get<map::tileModels>()->innerModel, tilesPosition, static_cast<int32_t>(it.count()));
        });
}

static void registerPostUpdateSystems(flecs::world &ecs)
{
    ecs.system<raylib::Camera3D>("endDrawing")
        .term_at(1).singleton()
        .kind(flecs::PostUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D * const camera) {
            camera->EndMode();

            ::DrawFPS(10, 10);

            ::EndDrawing();
    });
}

void registerSystems(flecs::world &ecs)
{
    registerOnStartSystems(ecs);
    registerPreUpdateSystems(ecs);
    registerOnUpdateSystems(ecs);
    registerPostUpdateSystems(ecs);
}

}
