//
// Created by quentinf on 14/06/24.
//
#include "server_to_gui_cmd_updating.hpp"

#include <cfloat>

#include "map.hpp"
#include "map_utils.hpp"
#include "player.hpp"
#include "raylib.h"

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
    raylib::ModelAnimation *const idle = &world.get<player::playerAnimations>()->animations->at(IDLE_ANIMATION_IDX);
    const flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(newPlayer->x, newPlayer->y));
    const auto &tileMatrix = tile.ensure<raylib::Matrix>();
    const player::playerTargetInfo playerPosInfo{
        .target = {tileMatrix.m12, tileMatrix.m14},
        .normalizedDirection = {0.0f, 0.0f},
    };

    player.set<Vector2>({tileMatrix.m12, tileMatrix.m14});
    player.set<player::playerTargetInfo>(playerPosInfo);
    player.disable<player::playerTargetInfo>();
    player.set<player::Orientation>(static_cast<player::Orientation>(newPlayer->orientation));
    player.set<uint8_t>(newPlayer->level);
    player.set<player::playerAnimationData>({idle, 0});
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

    raylib::ModelAnimation *const run = &world.get<player::playerAnimations>()->animations->at(RUN_ANIMATION_IDX);
    auto *const playerAnimData = player.get_mut<player::playerAnimationData>();
    playerAnimData->currentAnimation = run;
    playerAnimData->currentFrame = 0;

    auto &playerOrientation = player.ensure<player::Orientation>();
    playerOrientation = static_cast<player::Orientation>(playerPosition->orientation);

    auto *const playerTargetInfo = player.get_mut<player::playerTargetInfo>();
    playerTargetInfo->target = {tileMatrix.m12, tileMatrix.m14};

    auto playerPos = player.get_ref<Vector2>();
    Vector2 direction = {tileMatrix.m12 - playerPos->x, tileMatrix.m14 - playerPos->y};

    // Normalize the direction vector
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);  // TODO: Vector2Normalize
    if (std::fabsf(length) > FLT_EPSILON)
    {
        direction.x /= length;
        direction.y /= length;
    }
    playerTargetInfo->normalizedDirection = direction;
    player.enable<player::playerTargetInfo>();
    char dir;
    switch (playerPosition->orientation)
    {
        case 1:
            dir = 'N';
            break;
        case 2:
            dir = 'E';
            break;
        case 3:
            dir = 'S';
            break;
        case 4:
            dir = 'W';
            break;
        default:
            dir = 'U';
            break;
    }
    printf("Player %d moved to %d %d with %c orientation\n",
           playerPosition->id,
           playerPosition->x,
           playerPosition->y,
           dir);
}

}  // namespace zappy_gui::net
