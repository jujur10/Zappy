//
// Created by quentinf on 14/06/24.
//

#pragma once
#include "flecs.h"
#include "server_to_gui_cmd_structs.hpp"

namespace zappy_gui::net
{
/**
 * @brief Process the UpdateTileCommand command
 * Update the tile resources values and enable/disable the resource entities based on their values
 * @param world The ecs world
 * @param updateTile The received UpdateTileCommand command
 */
void HandleUpdateTileCommand(const flecs::world &world, const UpdateTileCommand *updateTile);

/**
 * @brief Process the NewPlayerCommand command
 * Create a new player entity and set its position, orientation, level and team name
 * @param world The ecs world
 * @param newPlayer The received NewPlayerCommand command
 */
void HandleNewPlayerCommand(const flecs::world &world, const NewPlayerCommand *newPlayer);

/**
 * @brief Process the DeadPlayerCommand command
 * Remove the player entity from the world
 * @param world The ecs world
 * @param deadPlayer The received DeadPlayerCommand command
 */
void HandleDeadPlayerCommand(const flecs::world &world, const DeadPlayerCommand *deadPlayer);

/**
 * @brief Process the PlayerPositionCommand command
 * Update the player entity position and orientation
 * @param world The ecs world
 * @param playerPosition The received PlayerPositionCommand command
 */
void HandlePlayerPositionCommand(const flecs::world &world, const PlayerPositionCommand *playerPosition);

/**
 * @brief Process the TimeUnitUpdateCommand command
 * Update the time unit value
 * @param world The ecs world
 * @param timeUnitUpdate The received TimeUnitUpdateCommand command
 */
void HandleTimeUnitUpdateCommand(const flecs::world &world, const TimeUnitUpdateCommand *timeUnitUpdate);

/**
 * @brief Process the StartIncantation command
 * Start the incantation animation
 * @param world The ecs world
 * @param newIncantation The received StartIncantationCommand command
 */
void HandleStartIncantationCommand(const flecs::world &world, const StartIncantationCommand *newIncantation);

/**
 * @brief Process the EndIncantation command
 * End the incantation animation
 * @param world The ecs world
 * @param endIncantation The received EndIncantationCommand command
 */
void HandleEndIncantationCommand(const flecs::world &world, const EndIncantationCommand *endIncantation);

/**
 * @brief Process the PlayerInventory command
 * Update the player inventory
 * @param world The ecs world
 * @param playerInventory The received PlayerInventoryCommand command
 */
void HandlePlayerInventoryCommand(const flecs::world &world, const PlayerInventoryCommand *playerInventory);
}  // namespace zappy_gui::net
