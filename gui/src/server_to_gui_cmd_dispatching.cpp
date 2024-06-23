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
            case 2:
            {
                if (auto const *const deadPlayer = std::get_if<DeadPlayerCommand>(&cmd))
                {
                    HandleDeadPlayerCommand(world, deadPlayer);
                }
                break;
            }
            case 3:
            {
                if (auto const *const playerMove = std::get_if<PlayerPositionCommand>(&cmd))
                {
                    HandlePlayerPositionCommand(world, playerMove);
                }
                break;
            }
            case 4:
            {
                if (auto const *const timeUnit = std::get_if<TimeUnitUpdateCommand>(&cmd))
                {
                    HandleTimeUnitUpdateCommand(world, timeUnit);
                }
                break;
            }
            case 5:
            {
                if (auto *const playerLevel = std::get_if<StartIncantationCommand>(&cmd))
                {
                    HandleStartIncantationCommand(world, playerLevel);
                }
                break;
            }
            case 6:
            {
                if (auto const *const incantationEnd = std::get_if<EndIncantationCommand>(&cmd))
                {
                    HandleEndIncantationCommand(world, incantationEnd);
                }
                break;
            }
            case 7:
            {
                if (auto const *const playerInventory = std::get_if<PlayerInventoryCommand>(&cmd))
                {
                    HandlePlayerInventoryCommand(world, playerInventory);
                }
                break;
            }
            case 8:
            {
                if (auto const *const playerPickup = std::get_if<PlayerPickupCommand>(&cmd))
                {
                    HandlePlayerPickupCommand(world, playerPickup);
                }
                break;
            }
            case 9:
            {
                if (auto const *const playerDrop = std::get_if<PlayerDropCommand>(&cmd))
                {
                    HandlePlayerDropCommand(world, playerDrop);
                }
                break;
            }
            case 10:
            {
                if (auto const *const teamName = std::get_if<TeamNameCommand>(&cmd))
                {
                    HandleTeamNameCommand(world, teamName);
                }
                break;
            }
            // Other command cases to be added here
            default:;
        }
    }
}
}  // namespace zappy_gui::net
