//
// Created by quentinf on 05/06/24.
//

#include "systems.hpp"

#include <flecs.h>

#include <Matrix.hpp>
#include <Camera3D.hpp>  // Must be included after Matrix.hpp, if not project will not compile

#include "map.hpp"
#include "raylib_utils.hpp"
#include "server_to_gui_cmd_handling.hpp"

namespace zappy_gui::systems
{

//----------------------------------------------------------------------------------
// Disables and Enables menu interactions
//----------------------------------------------------------------------------------
void HandleMenuInteraction(bool &menuInteraction)
{
    if (::IsKeyPressed(KEY_I))
    {
        menuInteraction = !menuInteraction;
        if (menuInteraction)
        {
            ::EnableCursor();
        }
        else
        {
            ::DisableCursor();
        }
    }
}

/// @brief Register systems that will be executed in the OnStart pipeline
static void registerOnStartSystems(const flecs::world &ecs)
{
    /// Generate the map
    ecs.system("GenerateMap").kind(flecs::OnStart).iter(map::GenerateMap);

    /// Query the tile models and load the instancing shader into them
    ecs.system<map::tileModels>("loadTileInstancingShader")
        .kind(flecs::OnStart)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, const map::tileModels *const models)
            {
                utils::SetupModel(models->grass, "gui/resources/shaders/tile_instancing.vs", nullptr);
            });

    ecs.system<map::resourceModels>("loadResourceInstancingShader")
        .kind(flecs::OnStart)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, const map::resourceModels *const models)
            {
                utils::SetupModel(models->food, "gui/resources/shaders/tile_instancing.vs", nullptr);
                utils::SetupModel(models->crystal, "gui/resources/shaders/tile_instancing.vs", nullptr);
            });
}

static void registerOnLoadSystems(const flecs::world &ecs)
{
    ecs.system("parseGuiCommand").kind(flecs::PreUpdate).iter(net::ParseGuiCommands);
}

static void registerPreUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera and setup the drawing
    ecs.system<raylib::Camera3D>("setupDrawing")
        .term_at(1)
        .singleton()
        .kind(flecs::PreUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera)
            {
                ::BeginDrawing();

                ::ClearBackground(BLACK);

                camera->BeginMode();
            });
}

static void registerOnUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera and update it using the keyboard and mouse inputs
    ecs.system<raylib::Camera3D>("updateCamera")
        .term_at(1)
        .singleton()
        .kind(flecs::OnUpdate)
        .iter([]([[maybe_unused]] const flecs::iter &it, raylib::Camera3D * const camera) {
            static bool menuInteraction = false;
            const static Vector3 nullVector = {0.0f, 0.0f, 0.0f}; // Is set as static so that it doesn't get deleted and recreated every frame
            HandleMenuInteraction(menuInteraction);
            camera->Update(
                Vector3{
                    (::IsKeyDown(KEY_W) || ::IsKeyDown(KEY_UP)) * 0.1f -    // Move forward-backward
                        (::IsKeyDown(KEY_S) || ::IsKeyDown(KEY_DOWN)) * 0.1f,
                    (::IsKeyDown(KEY_D) || ::IsKeyDown(KEY_RIGHT)) * 0.1f - // Move right-left
                        (::IsKeyDown(KEY_A) || ::IsKeyDown(KEY_LEFT)) * 0.1f,
                    0.0f                                                    // Move up-down
                },
                menuInteraction ? nullVector :
                Vector3{
                    ::GetMouseDelta().x * 0.05f, // Rotation: yaw
                    ::GetMouseDelta().y * 0.05f, // Rotation: pitch
                    0.0f                         // Rotation: roll
                },
                menuInteraction ? 0.0f :
                ::GetMouseWheelMove() * 2.0f);   // Move to target (zoom)
        });

    /// Query all the grass tiles and draw them
    ecs.system<raylib::Matrix>("drawTiles")
        .kind(flecs::OnUpdate)
        .without(map::resourceType::food)
        .without(map::resourceType::linemate)
        .without(map::resourceType::deraumere)
        .without(map::resourceType::sibur)
        .without(map::resourceType::mendiane)
        .without(map::resourceType::phiras)
        .without(map::resourceType::thystame)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const tilesPosition)
            {
                utils::DrawModelInstanced(it.world().get<map::tileModels>()->grass, tilesPosition, static_cast<int32_t>(it.count()));
            });

    /// Query all food and draw it
    ecs.system<raylib::Matrix>("drawFood")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::food)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const foodPosition)
            {
                utils::DrawModelInstanced(it.world().get<map::resourceModels>()->food, foodPosition, static_cast<int32_t>(it.count()));
            });

    /// Query all linemate and draw it
    ecs.system<raylib::Matrix>("drawLinemate")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::linemate)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const linematePosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    linematePosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::linemate) - 1]
                );
            });

    /// Query all deraumere and draw it
    ecs.system<raylib::Matrix>("drawDeraumere")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::deraumere)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const deraumerePosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    deraumerePosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::deraumere) - 1]
                );
            });

    /// Query all sibur and draw it
    ecs.system<raylib::Matrix>("drawSibur")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::sibur)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const siburPosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    siburPosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::sibur) - 1]
                );
            });

    /// Query all mendiane and draw it
    ecs.system<raylib::Matrix>("drawMendiane")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::mendiane)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const mendianePosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    mendianePosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::mendiane) - 1]
                );
            });

    /// Query all phiras and draw it
    ecs.system<raylib::Matrix>("drawPhiras")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::phiras)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const phirasPosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    phirasPosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::phiras) - 1]
                );
            });

    /// Query all thystame and draw it
    ecs.system<raylib::Matrix>("drawThystame")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::thystame)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const thystamePosition)
            {
                utils::DrawCrystals(
                    it.world().get<map::resourceModels>()->crystal,
                    thystamePosition,
                    static_cast<int32_t>(it.count()),
                    map::resourceColors[static_cast<int32_t>(map::resourceType::thystame) - 1]
                );
            });
}

static void registerPostUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera, end the drawing and display the FPS
    ecs.system<raylib::Camera3D>("endDrawing")
        .term_at(1)
        .singleton()
        .kind(flecs::PostUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera)
            {
                camera->EndMode();

                ::DrawFPS(10, 10);

                ::EndDrawing();
            });

    // TODO: Send a request to the server to update the map resources if map has not be updated for a while
    // ecs.system("AskForMapResourcesUpdate")
    //     .kind(flecs::PostUpdate)
    //     .iter(
    //         []([[maybe_unused]] const flecs::iter &it)
    //         {
    //             net::GuiToServerQueue.try_push(const_cast<char*>(GUI_MAP_TILES));
    //         });
}

void registerSystems(const flecs::world &ecs)
{
    registerOnStartSystems(ecs);
    registerOnLoadSystems(ecs);
    registerPreUpdateSystems(ecs);
    registerOnUpdateSystems(ecs);
    registerPostUpdateSystems(ecs);
}

}  // namespace zappy_gui::systems
