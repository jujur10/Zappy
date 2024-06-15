//
// Created by quentinf on 14/06/24.
//

#pragma once
#include <flecs.h>

#include "to_gui_commands.hpp"

namespace zappy_gui::net
{
/**
 * @brief Process the UpdateTileCommand command
 * Update the tile resources values and enable/disable the resource entities based on their values
 * @param world The ecs world
 * @param updateTile The received UpdateTileCommand command
 */
void HandleUpdateTileCommand(const flecs::world &world, const UpdateTileCommand &updateTile);
}
