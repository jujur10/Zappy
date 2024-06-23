//
// Created by quentinf on 10/06/24.
//

#include <cstdint>
#include <cstring>
#include <bit>
#include <stop_token>
#include <string>
#include <string_view>
#include <vector>
#include <print>

#include "my_write.hpp"
#include "networking.hpp"
#include "server_to_gui_cmd_structs.hpp"
#include "server_to_gui_cmd_value.hpp"
#include "gui_to_server_cmd_structs.hpp"
#include "sockets.hpp"

namespace zappy_gui::net
{
void HandleGuiCommand(const Socket& serverSocket)
{
    char* request = nullptr;
    while (GuiToServerQueue.try_pop(request) && request != nullptr)
    {
        serverSocket.Write(request, std::strlen(request));
        delete[] request;
    }
}

void HandleServerCommand(const std::string& line)
{
    const uint32_t command = *std::bit_cast<const uint32_t*>(line.c_str()) & 0x00'FF'FF'FFu;

    switch (static_cast<ServerCommands>(command))
    {
        case ServerCommands::MAP_SIZE:
        {
            break;
        }
        case ServerCommands::MAP_TILE:
        {
            ParseTileUpdateCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::NEW_PLAYER:
        {
            ParseNewPlayerCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::DEATH_OF_PLAYER:
        {
            ParseDeathOfPlayerCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_POSITION:
        {
            ParsePlayerPositionCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::TIME_UNIT_CHANGED:
        case ServerCommands::SET_TIME_UNIT:
        {
            ParseTimeUnitUpdatedCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::START_INCANTATION:
        {
            ParseStartIncantationCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::END_INCANTATION:
        {
            ParseEndIncantationCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_LEVEL:
        {
            ParsePlayerLevelCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_INVENTORY:
        {
            ParsePlayerInventoryCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_PICKUP:
        {
            ParsePlayerPickupCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_DROP:
        {
            ParsePlayerDropCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::TEAM_NAME:
        {
            ParseTeamNameCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::PLAYER_LAY_EGG: // Why would I care about this?
            break;
        case ServerCommands::EGG_LAID:
        {
            ParseEggLaidCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::CONNECTION_ON_EGG:
        {
            ParseConnectionOnEggCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        case ServerCommands::EGG_DEATH:
        {
            ParseDeathOfEggCommand(static_cast<std::string_view>(line).data() + 4);
            break;
        }
        default:
            std::print("Unknown command: {}\n", line);
            return;
    }
}

void NetworkTreadLoop(const std::stop_token& stoken, const Socket& serverSocket)
{
    const FileWriter errWriter(2);
    std::vector<char> responseBuffer;

    while (true)
    {
        HandleGuiCommand(serverSocket);

        std::string responseLine;
        while (!(responseLine = serverSocket.ReadLineFast(responseBuffer)).empty())
        {
            HandleServerCommand(responseLine);
        }
        if (stoken.stop_requested())
        {
            errWriter.writeNoReturn("Stop requested\n", 15);
            break;
        }
    }
}
}  // namespace zappy_gui::net
