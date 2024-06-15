//
// Created by quentinf on 13/06/24.
//

#include "gui_command_parsing.hpp"

#include <flecs.h>

#include <map.hpp>

#include "map_utils.hpp"
#include "to_gui_commands.hpp"

namespace zappy_gui::net
{


void ParseGuiCommands(const flecs::iter &it)
{
    Command cmd;
    flecs::world world = it.world();

    while (ServerToGuiQueue.try_pop(cmd))
    {
        switch (cmd.index())
        {
            case 0:
            {
                if (auto const *updateTile = std::get_if<UpdateTileCommand>(&cmd))
                {
                    flecs::entity tile = world.entity(utils::GetTileIndexFromCoords(updateTile->x, updateTile->y));

                    const auto *currentResourceIds = tile.get<map::resourceIds>();
                    if (currentResourceIds == nullptr)
                    {
                        return;
                    }

                    for (uint8_t i = 0; i < static_cast<uint8_t>(map::ressourceType::total); i++)
                    {
                        auto resourceEntity = world.entity(currentResourceIds->array[i]);
                        auto &currentResourceVal = resourceEntity.ensure<uint16_t>();
                        uint16_t newResourceVal = updateTile->resources[i];

                        if (currentResourceVal == newResourceVal)
                        {
                            continue;
                        }

                        currentResourceVal = newResourceVal;

                        if (currentResourceVal == 0 && resourceEntity.enabled())
                        {
                            resourceEntity.disable();
                        }
                        else if (currentResourceVal > 0 && !resourceEntity.enabled())
                        {
                            resourceEntity.enable();
                        }
                    }
                }
            }
            // Other command cases to be added here
            default:;
        }
    }
}
}  // namespace zappy_gui::net