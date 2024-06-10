//
// Created by quentinf on 10/06/24.
//

#include <my_write.hpp>

#include "commands.hpp"
#include "networking.hpp"

namespace zappy_gui::net
{
void NetworkTreadLoop(const std::stop_token& stoken, const Socket& serverSocket)
{
    const FileWriter errWriter(2);
    std::vector<char> responseBuffer;
    std::string errorMsg;

    while (true)
    {
        serverSocket.ReadLineTimeout(responseBuffer, 10, errorMsg);
        if (stoken.stop_requested())
        {
            errWriter.writeNoReturn("Stop requested\n", 15);
            break;
        }
    }
}
}  // namespace zappy_gui::net