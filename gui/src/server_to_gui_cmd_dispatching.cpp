//
// Created by quentinf on 13/06/24.
//

#include <variant>

#include "flecs.h"
#include "server_to_gui_cmd_handling.hpp"
#include "server_to_gui_cmd_structs.hpp"
#include "server_to_gui_cmd_updating.hpp"

namespace zappy_gui::net
{

void ParseGuiCommands(const flecs::iter &it)
{
    Command cmd;
    const flecs::world world = it.world();

    while (ServerToGuiQueue.try_pop(cmd))
    {
        switch (cmd.index())
        {
            case 0:
            {
                if (auto const *const updateTile = std::get_if<UpdateTileCommand>(&cmd))
                {
                    HandleUpdateTileCommand(world, updateTile);
                }
                break;
            }
            case 1:
            {
                if (auto const *const newPlayer = std::get_if<NewPlayerCommand>(&cmd))
                {
                    HandleNewPlayerCommand(world, newPlayer);
                }
                break;
            }
            // Other command cases to be added here
            default:;
        }
    }
}
}  // namespace zappy_gui::net
