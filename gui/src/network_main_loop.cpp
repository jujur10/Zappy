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
    }
}

void HandleServerCommand(const std::string& line)
{
    const uint32_t command = *std::bit_cast<const uint32_t*>(line.c_str()) & 0x00'FF'FF'FFu;

    switch (static_cast<ServerCommands>(command))
    {
        case ServerCommands::MAP_TILE:
        {
            ParseTileUpdateCommand(static_cast<std::string_view>(line).data() + 4);
        }
        default:
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
