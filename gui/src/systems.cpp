//
// Created by quentinf on 05/06/24.
//

#include "systems.hpp"

#include "map.hpp"
#include "raylib_utils.hpp"

#include <Camera3D.hpp>
#include <Matrix.hpp>
#include <flecs.h>
#include <rlgl.h>

namespace zappy_gui::systems
{

/// @brief Register systems that will be executed in the OnStart pipeline
static void registerOnStartSystems(const flecs::world &ecs)
{
    /// Generate the map
    ecs.system("generateMap").kind(flecs::OnStart).iter(map::generateMap);

    /// Query the tile models and load the instancing shader into them
    ecs.system<map::tileModels>("loadInstancingShader")
        .kind(flecs::OnStart)
        .iter([]([[maybe_unused]] const flecs::iter &it, const map::tileModels *const models) {
            utils::setupModel(models->outerModel, "gui/resources/shaders/tile_instancing.vs", nullptr);
            utils::setupModel(models->innerModel, "gui/resources/shaders/tile_instancing.vs", nullptr);
        });
}

static void registerPreUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera and setup the drawing
    ecs.system<raylib::Camera3D>("setupDrawing")
        .term_at(1)
        .singleton()
        .kind(flecs::PreUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera) {
            ::BeginDrawing();

            ::ClearBackground(BLACK);

            camera->BeginMode();

            ::rlDisableBackfaceCulling();
        });
}

static void registerOnUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera and update it using the keyboard and mouse inputs
    ecs.system<raylib::Camera3D>("updateCamera")
        .term_at(1)
        .singleton()
        .kind(flecs::OnUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera) {
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
                    0.0f                         // Rotation: roll
                },
                ::GetMouseWheelMove() * 2.0f);   // Move to target (zoom)
        });

    /// Query all the outer tiles and draw them
    ecs.system<raylib::Matrix, map::outerTile>("drawOuterTile")
        .kind(flecs::OnUpdate)
        .iter([](const flecs::iter &it, const raylib::Matrix *const tilesPosition, [[maybe_unused]] const map::outerTile *const type) {
            utils::drawModelInstanced(it.world().get<map::tileModels>()->outerModel, tilesPosition, static_cast<int32_t>(it.count()));
        });

    /// Query all the inner tiles and draw them
    ecs.system<raylib::Matrix, map::innerTile>("drawInnerTile")
        .kind(flecs::OnUpdate)
        .iter([](const flecs::iter &it, const raylib::Matrix *const tilesPosition, [[maybe_unused]] const map::innerTile *const type) {
            utils::drawModelInstanced(it.world().get<map::tileModels>()->innerModel, tilesPosition, static_cast<int32_t>(it.count()));
        });
}

static void registerPostUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera, end the drawing and display the FPS
    ecs.system<raylib::Camera3D>("endDrawing")
        .term_at(1)
        .singleton()
        .kind(flecs::PostUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera) {
            camera->EndMode();

            ::DrawFPS(10, 10);

            ::EndDrawing();
        });
}

void registerSystems(const flecs::world &ecs)
{
    registerOnStartSystems(ecs);
    registerPreUpdateSystems(ecs);
    registerOnUpdateSystems(ecs);
    registerPostUpdateSystems(ecs);
}

} // namespace zappy_gui::systems
