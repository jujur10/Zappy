//
// Created by quentinf on 05/06/24.
//

#include "systems.hpp"

#include <cfloat>
#include <cstring>
#include <memory>

#include "Matrix.hpp"
#include "Camera3D.hpp"  // Must be included after Matrix.hpp, if not project will not compile
#include "Mouse.hpp"
#include "Ray.hpp"
#include "flecs.h"
#include "gui.hpp"
#include "gui_to_server_cmd_structs.hpp"
#include "gui_to_server_cmd_value.hpp"
#include "map.hpp"
#include "map_utils.hpp"
#include "particle.hpp"
#include "player.hpp"
#include "raylib_utils.hpp"
#include "rlgl.h"  // Must be included after Matrix.hpp, if not project will not compile
#include "server_to_gui_cmd_handling.hpp"
#include "time_unit.hpp"

namespace zappy_gui::systems
{

//----------------------------------------------------------------------------------
// Disables and Enables menu interactions
//----------------------------------------------------------------------------------
void HandleMenuInteraction(bool &menuInteraction, const flecs::iter &it)
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
            it.world().entity<gui::Hover>().disable();
            it.world().entity<gui::Selection>().enable();
        }
    }
}

bool GetMouseCollisionWithTiles(const raylib::Ray &ray, Vector2 *const point)
{
    if (ray.direction.y < -0.0001f || ray.direction.y > 0.0001f)
    {
        const float distance = -ray.position.y / ray.direction.y;
        if (distance >= 0)
        {
            point->x = ray.position.x + ray.direction.x * distance;
            point->y = ray.position.z + ray.direction.z * distance;
            return true;
        }
    }
    return false;
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
                utils::SetupModel(models->sand, "gui/resources/shaders/tile_instancing.vs", nullptr);
                utils::SetupModel(models->sandRock, "gui/resources/shaders/tile_instancing.vs", nullptr);
                utils::SetupModel(models->sandCactus, "gui/resources/shaders/tile_instancing.vs", nullptr);
            });

    ecs.system<map::resourceModels>("loadResourceInstancingShader")
        .kind(flecs::OnStart)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, const map::resourceModels *const models)
            {
                utils::SetupModel(models->food, "gui/resources/shaders/tile_instancing.vs", nullptr);
                utils::SetupModel(models->crystal, "gui/resources/shaders/tile_instancing.vs", nullptr);
            });

    ecs.system<player::EggModel>("loadEggInstancingShader")
        .kind(flecs::OnStart)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, const player::EggModel *const model)
            {
                utils::SetupModel(model->model, "gui/resources/shaders/tile_instancing.vs", nullptr);
            });

    /// Ask server for time unit
    ecs.system("AskForTimeUnit")
        .kind(flecs::OnStart)
        .iter(
            []([[maybe_unused]] const flecs::iter &it)
            {
                auto *request = new char[5];
                std::memcpy(request, GUI_TIME_UNIT, 4);  // NOLINT
                request[4] = '\0';
                net::GuiToServerQueue.try_push(request);
            });
}

static void registerOnLoadSystems(const flecs::world &ecs)
{
    ecs.system("parseGuiCommand").kind(flecs::OnLoad).no_readonly().iter(net::ParseGuiCommands);

    ecs.system("checkForResizing")
        .kind(flecs::OnLoad)
        .no_readonly()
        .iter(
            [](const flecs::iter &it)
            {
                if (::IsWindowResized())
                {
                    auto *hoverEffect = it.world().get_mut<gui::HoverEffect>();
                    hoverEffect->renderTexture->Unload();
                    hoverEffect->renderTexture.reset(nullptr);
                    hoverEffect->renderTexture = std::make_unique<raylib::RenderTexture>(GetScreenWidth(), GetScreenHeight());

                    const int resolutionLoc = hoverEffect->sobolShader->GetLocation("resolution");
                    const Vector2 resolution = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
                    hoverEffect->sobolShader->SetValue(resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);
                }
            });

    ecs.system("registerClickPosition")
        .kind(flecs::OnLoad)
        .no_readonly()
        .iter(
            [](const flecs::iter &it)
            {
                const auto pos = raylib::Mouse::GetPosition();
                for (int buttonType = 0; buttonType <= 6; ++buttonType)
                {  // Iterate over all button types
                    if (raylib::Mouse::IsButtonReleased(buttonType))
                    {
                        it.world().entity().set<raylib::Vector2>(pos).add(static_cast<utils::MouseButton>(buttonType));
                    }
                }
            });

    ecs.system<raylib::Camera3D>("DetectHoveredTile")
        .kind(flecs::OnLoad)
        .term_at(1)
        .singleton()
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera)
            {
                if (it.world().has<gui::InUI>() || ::IsCursorHidden())
                {
                    return;
                }

                Vector2 point;
                if (!GetMouseCollisionWithTiles(camera->GetMouseRay(raylib::Mouse::GetPosition()), &point))
                {
                    return;
                }

                const Vector2 tileCoords = utils::GetCoordsFromVector(point);
                if (tileCoords.x < 0 || tileCoords.x >= map::kMAP_WIDTH || tileCoords.y < 0 || tileCoords.y >= map::kMAP_HEIGHT)
                {
                    it.world().entity<gui::Hover>().disable();
                    return;
                }

                auto tileIndex = utils::GetTileIndexFromCoords(tileCoords);
                const auto tile = it.world().entity(tileIndex);
                if (!tile.is_alive())
                {
                    it.world().entity<gui::Hover>().disable();
                    return;
                }

                const uint8_t tileType =
                    tile.has<map::tileType1>() ? 1 : (tile.has<map::tileType2>() ? 2 : (tile.has<map::tileType3>() ? 3 : 0));
                raylib::Model *model = nullptr;
                switch (tileType)
                {
                    case 1:
                        model = it.world().get<map::tileModelsNoShader>()->sand;
                        break;
                    case 2:
                        model = it.world().get<map::tileModelsNoShader>()->sandRock;
                        break;
                    case 3:
                        model = it.world().get<map::tileModelsNoShader>()->sandCactus;
                        break;
                    default:
                        model = it.world().get<map::tileModelsNoShader>()->sand;
                        break;
                }

                const auto *const tileMatrix = tile.get<raylib::Matrix>();
                const Vector3 tilePosition = {tileMatrix->m12, tileMatrix->m13, tileMatrix->m14};

                auto *const hover = it.world().get_mut<gui::Hover>();
                if (nullptr == hover)
                {
                    return;
                }
                hover->position->x = tilePosition.x;
                hover->position->y = tilePosition.y;
                hover->position->z = tilePosition.z;
                hover->model = model;
                it.world().entity<gui::Hover>().enable();

                const auto *const hoverData = it.world().get<gui::HoverEffect>();
                if (nullptr == hoverData)
                {
                    return;
                }

                hoverData->renderTexture->BeginMode();
                ::ClearBackground(BLANK);
                camera->BeginMode();
                model->Draw(tilePosition, 1.f, WHITE);
                camera->EndMode();
                hoverData->renderTexture->EndMode();
            });

    ecs.system<gui::Hover>("SelectEntity")
        .kind(flecs::OnLoad)
        .term_at(1)
        .singleton()
        .term<raylib::Vector2>()
        .with(utils::MouseButton::LeftButton)
        .iter(
            []([[maybe_unused]] const flecs::iter &it)
            {
                if (::IsCursorHidden())
                {
                    return;
                }

                auto const *const hover = it.world().get_mut<gui::Hover>();

                if (!it.world().entity<gui::Hover>().enabled() || nullptr == hover)
                {
                    return;
                }

                auto *const selection = it.world().get_mut<gui::Selection>();

                if (nullptr == selection)
                {
                    return;
                }

                selection->entityId = utils::GetTileIndexFromCoords(utils::GetCoordsFromVector(*hover->position));

                it.world().entity<gui::Selection>().enable();
            });
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

    /// Query the skybox and draw it
    ecs.system<map::skybox, raylib::Camera3D>("drawSkybox")
        .term_at(1)
        .singleton()
        .term_at(2)
        .singleton()
        .kind(flecs::PreUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, map::skybox const *const skybox, raylib::Camera3D const *const camera)
            {
                // We deactivate writing to the depth buffer when drawing the
                // skybox so that the skybox is always drawn behind everything
                ::rlDisableDepthMask();
                ::DrawModel(*skybox->model, camera->position, 1.0f, WHITE);
                ::rlEnableDepthMask();
            });
}

static void registerOnUpdateSystems(const flecs::world &ecs)
{
    /// Query the camera and update it using the keyboard and mouse inputs
    ecs.system<raylib::Camera3D>("updateCamera")
        .term_at(1)
        .singleton()
        .kind(flecs::OnUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera)
            {
                static bool menuInteraction = false;
                constexpr static Vector3 nullVector = {0.0f, 0.0f, 0.0f};  // Is static so that it doesn't get recreated every frame
                HandleMenuInteraction(menuInteraction, it);
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

    /// Query all the sand Type1 tiles and draw them
    ecs.system<raylib::Matrix>("drawType1Tiles")
        .kind(flecs::OnUpdate)
        .with<map::tileType1>()
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
                utils::DrawModelInstanced(it.world().get<map::tileModels>()->sand, tilesPosition, static_cast<int32_t>(it.count()));
            });

    /// Query all the sand Type2 tiles and draw them
    ecs.system<raylib::Matrix>("drawType2Tiles")
        .kind(flecs::OnUpdate)
        .with<map::tileType2>()
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
                utils::DrawModelInstanced(it.world().get<map::tileModels>()->sandRock, tilesPosition, static_cast<int32_t>(it.count()));
            });

    /// Query all the sand Type3 tiles and draw them
    ecs.system<raylib::Matrix>("drawType3Tiles")
        .kind(flecs::OnUpdate)
        .with<map::tileType3>()
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
                utils::DrawModelInstanced(it.world().get<map::tileModels>()->sandCactus, tilesPosition, static_cast<int32_t>(it.count()));
            });

    ecs.system<raylib::Camera3D, gui::HoverEffect>("DrawSelection")
        .kind(flecs::OnUpdate)
        .term_at(1)
        .singleton()
        .term_at(2)
        .singleton()
        .term<gui::Hover>()
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera, const gui::HoverEffect *const selection)
            {
                camera->EndMode();
                selection->sobolShader->BeginMode();
                ::DrawTextureRec(selection->renderTexture->texture,
                                 Rectangle{0,
                                           0,
                                           static_cast<float>(selection->renderTexture->texture.width),
                                           static_cast<float>(-selection->renderTexture->texture.height)},
                                 Vector2{0, 0},
                                 WHITE);
                selection->sobolShader->EndMode();
                camera->BeginMode();
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
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    linematePosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::linemate) - 1]);
            });

    /// Query all deraumere and draw it
    ecs.system<raylib::Matrix>("drawDeraumere")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::deraumere)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const deraumerePosition)
            {
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    deraumerePosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::deraumere) - 1]);
            });

    /// Query all sibur and draw it
    ecs.system<raylib::Matrix>("drawSibur")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::sibur)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const siburPosition)
            {
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    siburPosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::sibur) - 1]);
            });

    /// Query all mendiane and draw it
    ecs.system<raylib::Matrix>("drawMendiane")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::mendiane)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const mendianePosition)
            {
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    mendianePosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::mendiane) - 1]);
            });

    /// Query all phiras and draw it
    ecs.system<raylib::Matrix>("drawPhiras")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::phiras)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const phirasPosition)
            {
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    phirasPosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::phiras) - 1]);
            });

    /// Query all thystame and draw it
    ecs.system<raylib::Matrix>("drawThystame")
        .kind(flecs::OnUpdate)
        .with(map::resourceType::thystame)
        .iter(
            [](const flecs::iter &it, const raylib::Matrix *const thystamePosition)
            {
                utils::DrawCrystals(it.world().get<map::resourceModels>()->crystal,
                                    thystamePosition,
                                    static_cast<int32_t>(it.count()),
                                    map::resourceColors[static_cast<int32_t>(map::resourceType::thystame) - 1]);
            });

    /// Query the players animations and update them
    ecs.system<std::unique_ptr<raylib::Model>, player::PlayerAnimationData>("updatePlayerAnimations")
        .kind(flecs::OnUpdate)
        .each(
            [](std::unique_ptr<raylib::Model> const &model, player::PlayerAnimationData &animations)
            {
                model->UpdateAnimation(*animations.currentAnimation, animations.currentFrame);
                animations.currentFrame =
                    animations.currentAnimation->frameCount - 1 == animations.currentFrame ? 0 : animations.currentFrame + 1;
            });

    /// Query the players and update their position if they are moving
    ecs.system<Vector2, player::PlayerTargetInfo>("updatePlayerPosition")
        .kind(flecs::OnUpdate)
        .each(
            [](flecs::entity player, Vector2 &position, player::PlayerTargetInfo &targetInfo)
            {
                if (::Vector2Distance(position, targetInfo.target) < 0.1f)
                {
                END_POSITION_UPDATE:
                    player.disable<player::PlayerTargetInfo>();
                    player.enable<player::Orientation>();
                    auto *const playerAnimData = player.get_mut<player::PlayerAnimationData>();
                    playerAnimData->currentAnimation = &player.world().get<player::PlayerAnimations>()->animations->at(IDLE_ANIMATION_IDX);
                    playerAnimData->currentFrame = 0;
                    return;
                }

                float frequency;
                const auto * const timeUnit = player.world().get<TimeUnit>();

                if (nullptr == timeUnit)
                {
                    frequency = 100.f;
                }
                else
                {
                    frequency = static_cast<float>(timeUnit->frequency);
                }

                // Calculate the step size per frame
                const float step =
                    0.01f * (frequency / 2);  // Fixed step size per frame
                if (step > 0.2f)
                {
                    position = targetInfo.target;
                    goto END_POSITION_UPDATE;
                }
                position.x += targetInfo.normalizedDirection.x * step;
                position.y += targetInfo.normalizedDirection.y * step;

                const float halfTileSize = map::tileSize * 0.5f;

                const float yTopLimit = -static_cast<float>(map::kMAP_HEIGHT) * map::tileSize * 0.375f;
                const float yBottomLimit = static_cast<float>(map::kMAP_HEIGHT - 1) * map::tileSize * map::verticalSpacing -
                                           static_cast<float>(map::kMAP_HEIGHT) * map::tileSize * 0.375f;

                const float offsetX = utils::IsTileInOddRow(position.y) ? halfTileSize : 0.0f;
                const float xLeftLimit = (offsetX - static_cast<float>(map::kMAP_WIDTH) * halfTileSize) * map::spacing;
                const float xRightLimit = (static_cast<float>(map::kMAP_WIDTH - 1) * map::tileSize + offsetX -
                                           static_cast<float>(map::kMAP_WIDTH) * halfTileSize) *
                                          map::spacing;

                bool wrapAround = false;
                if (std::abs(position.y - (yTopLimit - halfTileSize)) <= 0.1f)
                {
                    position.y = yBottomLimit;
                    wrapAround = true;
                }
                else if (std::abs(position.y - (yBottomLimit + halfTileSize)) <= 0.1f)
                {
                    position.y = yTopLimit;
                    wrapAround = true;
                }

                if (std::abs(position.x - (xLeftLimit - halfTileSize)) <= 0.1f)
                {
                    position.x = xRightLimit;
                    wrapAround = true;
                }
                else if (std::abs(position.x - (xRightLimit + halfTileSize)) <= 0.1f)
                {
                    position.x = xLeftLimit;
                    wrapAround = true;
                }
                if (wrapAround)
                {
                    Vector2 direction = {targetInfo.target.x - position.x, targetInfo.target.y - position.y};
                    // Normalize the direction vector
                    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                    if (std::fabsf(length) > FLT_EPSILON)
                    {
                        direction.x /= length;
                        direction.y /= length;
                    }
                    targetInfo.normalizedDirection = direction;
                }
            });

    /// Query the players and update their rotation angle
    ecs.system<Vector2, player::Orientation, float>("updatePlayersRotation")
        .kind(flecs::OnUpdate)
        .each(
            [](const Vector2 &position, player::Orientation const &orientation, float &rotationAngle)
            {
                switch (orientation)
                {
                    using enum player::Orientation;
                    case kNorth:
                        rotationAngle = utils::IsTileInOddRow(position.y) ? NORTH_ODD_ANGLE : NORTH_REGULAR_ANGLE;
                        break;
                    case kEast:
                        rotationAngle = EAST_ANGLE;
                        break;
                    case kSouth:
                        rotationAngle = utils::IsTileInOddRow(position.y) ? SOUTH_ODD_ANGLE : SOUTH_REGULAR_ANGLE;
                        break;
                    case kWest:
                        rotationAngle = WEST_ANGLE;
                        break;
                    default:
                        break;
                }
            });

    /// Query the players and draw them
    ecs.system<Vector2, std::unique_ptr<raylib::Model>, float>("drawPlayers")
        .kind(flecs::OnUpdate)
        .each(
            [](const Vector2 &position, const std::unique_ptr<raylib::Model> &model, const float &rotationAngle)
            {
                model->Draw({position.x, 0.2f, position.y}, {0.f, 1.f, 0.f}, rotationAngle, {1.f, 1.f, 1.f}, WHITE);
            });

    /// Query the incantation info and update the icon to camera distance
    ecs.system<raylib::Camera3D, raylib::Matrix, player::IncantationInfo>("Update incantation icon distance")
        .term_at(1)
        .singleton()
        .kind(flecs::OnUpdate)
        .each(
            [](const raylib::Camera3D &camera, const raylib::Matrix &incantationPos, player::IncantationInfo &incantationInfo)
            {
                incantationInfo.distance = ::Vector3Distance(camera.position, {incantationPos.m12, incantationPos.m13, incantationPos.m14});
            });

    /// Update the incantation icon
    ecs.system<player::IncantationInfo>("Update incantation icon")
        .kind(flecs::OnUpdate)
        .each(
            [](flecs::entity e, player::IncantationInfo &incantationInfo)
            {
                if (incantationInfo.state == player::IncantationState::kInProgress)
                {
                    return;
                }
                if (0 == incantationInfo.frameLeftForIcon)
                {
                    e.remove<player::IncantationInfo>();
                    return;
                }
                --incantationInfo.frameLeftForIcon;
            });

    /// Draw the incantation icon
    ecs.system<raylib::Camera3D, player::IncantationIcons, raylib::Matrix, player::IncantationInfo>("draw incantation icon")
        .kind(flecs::OnUpdate)
        .term_at(1)
        .singleton()
        .term_at(2)
        .singleton()
        .order_by<player::IncantationInfo>(
            []([[maybe_unused]] const flecs::entity_t e1,
               const player::IncantationInfo *const iI1,
               [[maybe_unused]] const flecs::entity_t e2,
               const player::IncantationInfo *const iI2)
            {
                return (iI1->distance > iI2->distance) - (iI1->distance < iI2->distance);
            })
        .each(
            [](const raylib::Camera3D &camera,
               const player::IncantationIcons &icons,
               const raylib::Matrix &incantationPos,
               const player::IncantationInfo &incantationInfo)
            {
                icons.icons[static_cast<unsigned long>(incantationInfo.state)]->DrawBillboard(
                    camera, Vector3{incantationPos.m12, incantationPos.m13 + 1.5f, incantationPos.m14}, 0.5f);
            });

    /// Draw the eggs
    ecs.system<player::EggModel, raylib::Matrix>("drawEggs")
        .kind(flecs::OnUpdate)
        .term_at(1)
        .singleton()
        .with<player::EggData>()
        .iter(
            [](const flecs::iter &it, const player::EggModel *const model, const raylib::Matrix *const eggsPosition)
            {
                utils::DrawModelInstanced(model[0].model, eggsPosition, static_cast<int32_t>(it.count()));
            });
}

static void registerPostUpdateSystems(flecs::world const &ecs)
{
    /// Query the camera, end the drawing and display the FPS
    ecs.system<raylib::Camera3D>("endCamera")
        .term_at(1)
        .singleton()
        .kind(flecs::PostUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, raylib::Camera3D *const camera)
            {
                camera->EndMode();
                ::DrawFPS(10, 10);
            });

    ecs.system<std::unique_ptr<WinningEffect>>("DrawWinningEffect")
        .kind(flecs::PostUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it, const std::unique_ptr<WinningEffect> *effect)
            {
                effect[0]->Update(::GetFrameTime());
                effect[0]->Draw();
            });

    ecs.system("drawFloatingWindows")
        .kind(flecs::PostUpdate)
        .iter(
            [](const flecs::iter &it)
            {
                gui::WorldFloatingWindowMenu(it.world());
                gui::TileFloatingWindowMenu(it.world());
            });

    ecs.system("endDrawing")
        .kind(flecs::PostUpdate)
        .iter(
            []([[maybe_unused]] const flecs::iter &it)
            {
                ::EndDrawing();
            });

    /// Query the mouse clicks events and delete them
    ecs.system("deleteMouseClicks")
        .kind(flecs::PostUpdate)
        .with<raylib::Vector2>()
        .with(utils::MouseButton::LeftButton)
        .or_()
        .with(utils::MouseButton::RightButton)
        .or_()
        .with(utils::MouseButton::MiddleButton)
        .or_()
        .with(utils::MouseButton::SideButton)
        .or_()
        .with(utils::MouseButton::ExtraButton)
        .or_()
        .with(utils::MouseButton::ForwardButton)
        .or_()
        .with(utils::MouseButton::BackButton)
        .each(
            [](flecs::entity const &entity)
            {
                entity.destruct();
            });
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
