//
// Created by quentinf on 13/06/24.
//

#include "gui_command_parsing.hpp"

#include <flecs.h>

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
                    uint8_t counter = 0;
                    uint8_t counter2 = 0;
                    tile.children(
                        [&counter, &updateTile, &counter2](flecs::entity child)
                        {
                            if (counter2 >= 6)
                            {
                                printf("tile %d %d is full\n", updateTile->x, updateTile->y);
                            }
                            if (counter >= 6)
                            {
                                return;
                            }
                            *child.get_mut<uint16_t>() = updateTile->resources[counter];
                            // if (updateTile->resources[counter] == 0 && child.enabled())
                            // {
                            // }
                            //     child.disable();
                            if (updateTile->resources[counter] > 0)
                            {
                                counter2++;
                                child.enable();
                            }
                            counter++;
                        });
                }
            }
        }
    }
}
}  // namespace zappy_gui::net