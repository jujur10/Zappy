//
// Created by quentinf on 14/06/24.
//

#include "gui_command_update_handling.hpp"

#include "map.hpp"
#include "map_utils.hpp"

namespace zappy_gui::net
{
void HandleUpdateTileCommand(const flecs::world &world, const UpdateTileCommand &updateTile)
{
    const flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(updateTile.x, updateTile.y));
    const auto *const currentResourceIds = tile.get<map::resourceIds>();
    if (nullptr == currentResourceIds)
    {
        return;
    }

    // Iterate over the all the tile resources and update their values
    for (uint8_t i = 0; i < static_cast<uint8_t>(map::ressourceType::total); ++i)
    {
        auto resourceEntity = world.entity(currentResourceIds->array[i]);
        auto &currentResourceVal = resourceEntity.ensure<uint16_t>();
        const uint16_t newResourceVal = updateTile.resources[i];

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
}  // namespace zappy_gui::net
