//
// Created by quentinf on 14/06/24.
//
#include "server_to_gui_cmd_updating.hpp"

#include <Camera3D.hpp>
#include <cfloat>

#include "map.hpp"
#include "map_utils.hpp"
#include "player.hpp"
#include "raylib.h"
#include "time_unit.hpp"

namespace zappy_gui::net
{
void HandleUpdateTileCommand(const flecs::world &world, const UpdateTileCommand *const updateTile)
{
    const flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(updateTile->x, updateTile->y));
    const auto *const currentResourceIds = tile.get<map::resourceIds>();
    if (nullptr == currentResourceIds)
    {
        return;
    }

    // Iterate over the all the tile resources and update their values
    for (uint8_t i = 0; i < static_cast<uint8_t>(map::resourceType::total); ++i)
    {
        auto resourceEntity = world.entity(currentResourceIds->array[i]);
        auto &currentResourceVal = resourceEntity.ensure<uint16_t>();
        const uint16_t newResourceVal = updateTile->resources[i];

        if (currentResourceVal == newResourceVal)
        {
            continue;
        }

        currentResourceVal = newResourceVal;

        // Enable or disable the resource entity based on its value (for optimization)
        if (0 == currentResourceVal && resourceEntity.enabled())
        {
            resourceEntity.disable();
        }
        else if (currentResourceVal > 0 && !resourceEntity.enabled())
        {
            resourceEntity.enable();
        }
    }
}

void HandleNewPlayerCommand(const flecs::world &world, const NewPlayerCommand *const newPlayer)
{
    flecs::entity player = world.make_alive(newPlayer->id + PLAYER_STARTING_IDX);
    raylib::ModelAnimation *const idle = &world.get<player::PlayerAnimations>()->animations->at(IDLE_ANIMATION_IDX);
    const flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(newPlayer->x, newPlayer->y));
    const auto &tileMatrix = tile.ensure<raylib::Matrix>();
    const player::PlayerTargetInfo playerPosInfo{
        .target = {tileMatrix.m12, tileMatrix.m14},
        .normalizedDirection = {0.0f, 0.0f},
    };

    player.set<Vector2>({tileMatrix.m12, tileMatrix.m14});
    player.set<float>(0.f);
    player.set<player::PlayerTargetInfo>(playerPosInfo);
    player.disable<player::PlayerTargetInfo>();
    player.set<player::Orientation>(static_cast<player::Orientation>(newPlayer->orientation));
    player.set<uint8_t>(newPlayer->level);
    player.set<player::PlayerAnimationData>({idle, 0});
    player.set<std::unique_ptr<raylib::Model>>(std::make_unique<raylib::Model>("gui/resources/assets/cactoro.m3d"));
    player.set<std::string_view>(newPlayer->teamName);  // TODO: Don't forget this when implementing teams
}

void HandleDeadPlayerCommand(const flecs::world &world, const DeadPlayerCommand *const deadPlayer)
{
    if (world.entity(deadPlayer->id + PLAYER_STARTING_IDX).is_alive())
    {
        world.entity(deadPlayer->id + PLAYER_STARTING_IDX).destruct();
    }
}

void HandlePlayerPositionCommand(const flecs::world &world, const PlayerPositionCommand *const playerPosition)
{
    flecs::entity player = world.entity(playerPosition->id + PLAYER_STARTING_IDX);
    if (!player.is_alive())
    {
        return;
    }

    const flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(playerPosition->x, playerPosition->y));
    const auto &tileMatrix = tile.ensure<raylib::Matrix>();

    auto &playerOrientation = player.ensure<player::Orientation>();
    playerOrientation = static_cast<player::Orientation>(playerPosition->orientation);

    auto playerPos = player.get_ref<Vector2>();

    // if player just changed it's orientation skip rest of the code
    if (::Vector2Distance(*playerPos.get(), {tileMatrix.m12, tileMatrix.m14}) <= 0.1f)
    {
        player.disable<player::PlayerTargetInfo>();
        player.enable<player::Orientation>();
        return;
    }

    auto *const playerTargetInfo = player.get_mut<player::PlayerTargetInfo>();
    playerTargetInfo->target = {tileMatrix.m12, tileMatrix.m14};

    Vector2 direction = {tileMatrix.m12 - playerPos->x, tileMatrix.m14 - playerPos->y};

    // Normalize the direction vector
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (std::fabsf(length) > FLT_EPSILON)
    {
        direction.x /= length;
        direction.y /= length;
    }
    playerTargetInfo->normalizedDirection = direction;

    // Current player position (x,y)
    const Vector2 playerPosIndices = utils::GetCoordsFromVector(Vector2{playerPos->x, playerPos->y});

    // Get the difference between the current player position and the new one
    const float diffX = std::fabsf(playerPosIndices.x - static_cast<float>(playerPosition->x));
    const float diffY = std::fabsf(playerPosIndices.y - static_cast<float>(playerPosition->y));

    // If the player moved more than 1 in X or Y, it means he went around the map so update the inverse the direction to go out of the map
    playerTargetInfo->normalizedDirection = {
        direction.x * (diffX > 1 ? -1.0f : 1.0f),
        direction.y * (diffY > 1 ? -1.0f : 1.0f)
    };

    /// Enable the player target info component so that it can match the update position system
    player.enable<player::PlayerTargetInfo>();

    /// Force the player to face the right direction while running
    auto * const rotationAngle = player.get_mut<float>();
    switch (playerOrientation)
    {
        using enum player::Orientation;
        case kNorth:
            // Make the player face the right direction while running (which is the opposite of the real orientation)
            // disable player orientation component so that the system doesn't re rotate the player in it's real orientation
            *rotationAngle = !utils::IsTileInOddRow(tileMatrix.m14) ? SOUTH_ODD_ANGLE : SOUTH_REGULAR_ANGLE;
            player.disable<player::Orientation>();
            break;
        case kEast:
            *rotationAngle = EAST_ANGLE;
            break;
        case kSouth:
            // Make the player face the right direction while running (which is the opposite of the real orientation)
            // disable player orientation component so that the system doesn't re rotate the player in it's real orientation
            *rotationAngle = !utils::IsTileInOddRow(tileMatrix.m14) ? NORTH_ODD_ANGLE : NORTH_REGULAR_ANGLE;
            player.disable<player::Orientation>();
            break;
        case kWest:
            *rotationAngle = WEST_ANGLE;
            break;
        default:
            break;
    }

    /// Update the player animation to the run animation
    raylib::ModelAnimation *const run = &world.get<player::PlayerAnimations>()->animations->at(RUN_ANIMATION_IDX);
    auto *const playerAnimData = player.get_mut<player::PlayerAnimationData>();
    playerAnimData->currentAnimation = run;
    playerAnimData->currentFrame = 0;
}

void HandleTimeUnitUpdateCommand(const flecs::world &world, const TimeUnitUpdateCommand *const timeUnitUpdate)
{
    world.ensure<TimeUnit>().frequency = timeUnitUpdate->timeUnit;
}

void HandleStartIncantationCommand(const flecs::world &world, const StartIncantationCommand * const newIncantation)
{
    auto tile = world.entity(utils::GetTileIndexFromCoords(newIncantation->x, newIncantation->y));
    const auto * const tileMatrix = tile.get<raylib::Matrix>();
    auto const &cameraPos = world.get_ref<raylib::Camera3D>()->position;

    tile.set<player::IncantationInfo>({
        .incantationTexture = std::make_unique<raylib::Texture2D>("gui/resources/assets/hourglass.png"),
        .state = player::IncantationState::kInProgress,
        .frameLeftForIcon = 60,
        .distance = ::Vector3Distance(cameraPos, {tileMatrix->m12, tileMatrix->m13 + 1.5f, tileMatrix->m14}),
    });
}

}  // namespace zappy_gui::net
