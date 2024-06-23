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
    auto team = world.lookup(newPlayer->teamName.get()).id();
    if (0 == team)
    {
        team = world.entity(newPlayer->teamName.get()).id();
    }
    player.set<uint64_t>(team);
    player.set<player::PlayerInventory>({10, 0, 0, 0, 0, 0, 0});
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

    auto * const playerPos = player.get_mut<Vector2>();
    if (nullptr == playerPos)
    {
        return;
    }

    // if there is a player target info component, it means the player is running so teleport the player to the target position
    if (player.has<player::PlayerTargetInfo>() && player.enabled<player::PlayerTargetInfo>())
    {
        *playerPos = player.get_ref<player::PlayerTargetInfo>()->target;
    }

    // if player just changed it's orientation skip rest of the code
    if (::Vector2Distance(*playerPos, {tileMatrix.m12, tileMatrix.m14}) <= 0.1f)
    {
        player.disable<player::PlayerTargetInfo>();
        player.enable<player::Orientation>();
        player.set<player::Orientation>(static_cast<player::Orientation>(playerPosition->orientation));
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

    auto &playerOrientation = player.ensure<player::Orientation>();
    /// Force the player to face the right direction while running
    auto * const rotationAngle = player.get_mut<float>();
    switch (playerOrientation)
    {
        using enum player::Orientation;
        case kNorth:
            // Make the player face the right direction while running (which is the opposite of the real orientation)
            // disable player orientation component so that the system doesn't re rotate the player in it's real orientation
            *rotationAngle = static_cast<uint16_t>(playerPosIndices.y) % 2 ? NORTH_ODD_ANGLE : NORTH_REGULAR_ANGLE;
            player.disable<player::Orientation>();
            break;
        case kEast:
            *rotationAngle = EAST_ANGLE;
            break;
        case kSouth:
            // Make the player face the right direction while running (which is the opposite of the real orientation)
            // disable player orientation component so that the system doesn't re rotate the player in it's real orientation
            *rotationAngle = static_cast<uint16_t>(playerPosIndices.y) % 2 ? SOUTH_ODD_ANGLE : SOUTH_REGULAR_ANGLE;
            player.disable<player::Orientation>();
            break;
        case kWest:
            *rotationAngle = WEST_ANGLE;
            break;
        default:
            break;
    }
    playerOrientation = static_cast<player::Orientation>(playerPosition->orientation);

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

void HandleStartIncantationCommand(const flecs::world &world, StartIncantationCommand * const newIncantation)
{
    auto tile = world.entity(utils::GetTileIndexFromCoords(newIncantation->x, newIncantation->y));
    const auto * const tileMatrix = tile.get<raylib::Matrix>();
    auto const &cameraPos = world.get_ref<raylib::Camera3D>()->position;

    tile.set<player::IncantationInfo>({
        .state = player::IncantationState::kInProgress,
        .frameLeftForIcon = 120,
        .nbPlayers = newIncantation->playerCount,
        .distance = ::Vector3Distance(cameraPos, {tileMatrix->m12, tileMatrix->m13 + 1.5f, tileMatrix->m14}),
        .playerIds = std::move(newIncantation->playerIds)
    });
}

void HandleEndIncantationCommand(const flecs::world &world, const EndIncantationCommand * const endIncantation)
{
    const auto incantationTile = world.entity(utils::GetTileIndexFromCoords(endIncantation->x, endIncantation->y));
    if (!incantationTile.is_alive())
    {
        return;
    }
    const auto *const tileMatrix = incantationTile.get<raylib::Matrix>();

    auto * const incantationInfo = incantationTile.get_mut<player::IncantationInfo>();
    if (nullptr == incantationInfo)
    {
        return;
    }

    auto const &cameraPos = world.get_ref<raylib::Camera3D>()->position;

    incantationInfo->state = endIncantation->success ? player::IncantationState::kSuccess : player::IncantationState::kFailure;
    incantationInfo->frameLeftForIcon = 120;
    incantationInfo->distance = ::Vector3Distance(cameraPos, {tileMatrix->m12, tileMatrix->m13 + 1.5f, tileMatrix->m14});

    const auto * const playerIds = incantationInfo->playerIds.get();
    const auto playerCount = incantationInfo->nbPlayers;

    for (uint16_t i = 0; i < playerCount; ++i)
    {
        const auto player = world.entity(playerIds[i] + PLAYER_STARTING_IDX);
        if (!player.is_alive())
        {
            continue;
        }
        auto * const level = player.get_mut<uint8_t>();
        if (level != nullptr && *level < 8)
        {
            *level += 1;
        }
    }
}

void HandlePlayerLevelCommand(const flecs::world &world, const PlayerLevelCommand * const playerLevel)
{
    auto player = world.entity(playerLevel->id + PLAYER_STARTING_IDX);
    if (!player.is_alive())
    {
            return;
    }
    player.set<uint8_t>(playerLevel->level);
}


void HandlePlayerInventoryCommand(const flecs::world &world, const PlayerInventoryCommand *const playerInventory)
{
    const auto player = world.entity(playerInventory->id + PLAYER_STARTING_IDX);
    if (!player.is_alive())
    {
        return;
    }

    auto &playerResources = player.ensure<player::PlayerInventory>();
    for (uint8_t i = 0; i < static_cast<uint8_t>(map::resourceType::total); ++i)
    {
        playerResources.resources[i] = playerInventory->resources[i];
    }
}

void HandlePlayerPickupCommand(const flecs::world &world, const PlayerPickupCommand *const playerPickup)
{
    const auto player = world.entity(playerPickup->id + PLAYER_STARTING_IDX);
    if (!player.is_alive())
    {
        return;
    }

    auto &playerResources = player.ensure<player::PlayerInventory>();
    playerResources.resources[playerPickup->resource] += 1;
}

void HandlePlayerDropCommand(const flecs::world &world, const PlayerDropCommand *const playerDrop)
{
    const auto player = world.entity(playerDrop->id + PLAYER_STARTING_IDX);
    if (!player.is_alive())
    {
        return;
    }

    auto &playerResources = player.ensure<player::PlayerInventory>();
    playerResources.resources[playerDrop->resource] -= 1;
}

void HandleTeamNameCommand(const flecs::world &world, const TeamNameCommand *const teamName)
{
    world.entity(teamName->teamName.get());
}

void HandleEggLaidCommand(const flecs::world &world, const EggLaidCommand *const eggLaid)
{
    const auto tile = world.entity(utils::GetTileIndexFromCoords(eggLaid->x, eggLaid->y));
    if (!tile.is_alive())
    {
        return;
    }
    const auto * const tileMatrix = tile.get<raylib::Matrix>();
    auto egg = world.make_alive(eggLaid->eggId + EGG_STARTING_IDX);
    egg.set<player::EggData>({eggLaid->playerId});
    egg.set<raylib::Matrix>(raylib::Matrix::Translate(tileMatrix->m12, tileMatrix->m13 + 0.2f, tileMatrix->m14));
}

void HandleConnectionOnEggCommand(const flecs::world &world, const ConnectionOnEggCommand *const connectionOnEgg)
{
    const auto egg = world.entity(connectionOnEgg->eggId + EGG_STARTING_IDX);
    if (!egg.is_alive())
    {
        return;
    }
    egg.destruct();
}

void HandleDeathOfEggCommand(const flecs::world &world, const DeathOfEggCommand *const deathOfEgg)
{
    const auto egg = world.entity(deathOfEgg->eggId + EGG_STARTING_IDX);
    if (!egg.is_alive())
    {
        return;
    }
    egg.destruct();
}
}  // namespace zappy_gui::net
