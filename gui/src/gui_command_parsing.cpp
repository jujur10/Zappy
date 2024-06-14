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

                    tile.children(
                        [&updateTile](flecs::entity child)
                        {
                            const auto *type = child.get<map::ressourceType>();
                            if (type == nullptr)
                            {
                                return;
                            }

                            const auto newResourceVal = updateTile->resources[static_cast<int32_t>(*type)];
                            auto &currentResourceVal = child.ensure<uint16_t>();
                            // printf("type: %s\n", child.type().str().c_str());
                            if (currentResourceVal == newResourceVal)
                            {
                                return;
                            }

                            currentResourceVal = newResourceVal;

                            if (currentResourceVal == 0 && child.enabled())
                            {
                                child.disable();
                            }
                            else if (currentResourceVal > 0 && !child.enabled())
                            {
                                child.enable();
                            }
                        });
                }
            }
            default:;
        }
    }
}
}  // namespace zappy_gui::net