//
// Created by quentinf on 15/06/24.
//

#include "server_handshake.hpp"

#include "map.hpp"
#include "my_exit.hpp"
#include "my_write.hpp"
#include "server_to_gui_cmd_value.hpp"
#include "sockets.hpp"
#include "string_utils.hpp"

namespace zappy_gui
{

namespace map // global variables
{
int32_t kMAP_WIDTH;
int32_t kMAP_HEIGHT;
}  // namespace map


void Handshake(const Socket &serverSocket)
{
    const FileWriter errWriter(2);
    std::vector<char> responseBuffer;
    std::string errorMsg;

    // Lambda to exit with an error message
    auto exitWithError = [&errWriter](const std::string &msg)
    {
        errWriter.writeNoReturn(msg.c_str(), msg.length());
        SystemExit::exit(1);
    };

    // Lambda to read a line from the server and check for errors
    auto readLineAndCheck = [&serverSocket, &responseBuffer, &errorMsg, &exitWithError]
    {
        std::string line = serverSocket.ReadLineTimeout(responseBuffer, 1'000, errorMsg);
        if (line.empty())
        {
            exitWithError(errorMsg);
        }
        return line;
    };

    std::string responseLine = readLineAndCheck();
    if (responseLine != "WELCOME" || !responseBuffer.empty())
    {
        exitWithError(
            "Failed handshake with server, did not receive welcome or received "
            "too much data");
    }

    if (serverSocket.Write(static_cast<const char *>("GRAPHIC\n"), 8) == -1)
    {
        exitWithError("Failed handshake with server at writing team name");
    }

    uint16_t clientNumber = 0;
    responseLine = readLineAndCheck();

    // Check if the response is a command or a client number
    const uint32_t command = *std::bit_cast<const uint32_t *>(responseLine.c_str()) & 0x00'FF'FF'FFu;
    if (command == static_cast<uint32_t>(net::ServerCommands::MAP_SIZE))
    {
        // Epitech test server doesn't follow the full AI protocol
        // They send no client number and no map size
        // Instead they send the commands: msz, sgt
        // After that they send bct * tile number every 200ms
        const std::string args = responseLine.substr(4);

        const size_t mapWidthIdx = args.find(' ');

        int32_t mapWidth = 0;
        int32_t mapHeight = 0;
        if (!string_utils::convertFromString(static_cast<std::string_view>(args).substr(0, mapWidthIdx), mapWidth) ||
            !string_utils::convertFromString(static_cast<std::string_view>(args).substr(mapWidthIdx + 1), mapHeight))
        {
            exitWithError("Failed to convert map dimensions from string");
        }
        if (mapWidth <= 0 || mapHeight <= 0)
        {
            exitWithError("Failed handshake with server, invalid map size");
        }

        map::kMAP_WIDTH = mapWidth;
        map::kMAP_HEIGHT = mapHeight;
        return;
    }
    if (!string_utils::convertFromString(responseLine, clientNumber))
    {
        exitWithError("Failed to convert client number from string");
    }
    if (0 == clientNumber)
    {
        exitWithError("Failed handshake with server, no slot left");
    }

    responseLine = readLineAndCheck();
    const size_t mapWidthIdx = responseLine.find(' ');
    if (mapWidthIdx == std::string::npos)
    {
        exitWithError(
            "Failed handshake with server, missing space between map_width and "
            "map_height");
    }

    int32_t mapWidth = 0;
    int32_t mapHeight = 0;
    if (!string_utils::convertFromString(static_cast<std::string_view>(responseLine).substr(0, mapWidthIdx), mapWidth) ||
        !string_utils::convertFromString(static_cast<std::string_view>(responseLine).substr(mapWidthIdx + 1), mapHeight))
    {
        exitWithError("Failed to convert map dimensions from string");
    }
    if (mapWidth <= 0 || mapHeight <= 0)
    {
        exitWithError("Failed handshake with server, invalid map size");
    }

    map::kMAP_WIDTH = mapWidth;
    map::kMAP_HEIGHT = mapHeight;
}
}  // namespace zappy_gui
