//
// Created by quentinf on 10/06/24.
//

#include <cstring>
#include <my_write.hpp>

#include "networking.hpp"
#include "to_gui_commands.hpp"
#include "to_server_command.hpp"
#include "server_commands.hpp"

namespace zappy_gui::net
{
void HandleGuiCommand(const Socket& serverSocket)
{
    char* request = nullptr;
    while (!GuiToServerQueue.try_pop(request) && request != nullptr)
    {
        serverSocket.Write(request, std::strlen(request));
    }
}

void HandleServerCommand(const Socket& serverSocket, std::string& line)
{
    const uint32_t command = *std::bit_cast<const uint32_t *>(line.c_str()) & 0x00'FF'FF'FFu;

    switch (static_cast<ServerCommands>(command))
    {
        case ServerCommands::MAP_TILE:
        {
            ParseTileUpdateCommand(static_cast<std::string_view>(line).substr(3));
        }
        default:
            return;
    }
}

void NetworkTreadLoop(const std::stop_token& stoken, const Socket& serverSocket)
{
    const FileWriter errWriter(2);
    std::vector<char> responseBuffer;
    std::string errorMsg;

    while (true)
    {
        HandleGuiCommand(serverSocket);
        serverSocket.ReadLineTimeout(responseBuffer, 10, errorMsg);
        if (stoken.stop_requested())
        {
            errWriter.writeNoReturn("Stop requested\n", 15);
            break;
        }
    }
}
}  // namespace zappy_gui::net
