//
// Created by quentinf on 10/06/24.
//

#include "server_commands.hpp"
#include "string_utils.hpp"
#include "to_gui_commands.hpp"

namespace zappy_gui::net
{
void ParseTileUpdateCommand(const std::string_view& line)
{
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t resources[7] = {99};
    size_t paramPos = 0;

    string_utils::convertFromString(line, x);

    paramPos = line.find_first_of(' ') + 1;
    string_utils::convertFromString(line.data() + paramPos, y);

    paramPos = line.find_first_of(' ', paramPos) + 1;

    for (uint16_t& resource : resources)
    {
        string_utils::convertFromString(line.data() + paramPos, resource);
        paramPos = line.find_first_of(' ', paramPos) + 1;
    }
    ServerToGuiQueue.try_push(UpdateTileCommand{.x = x,
                                            .y = y,
                                            .resources = {resources[0],
                                                          resources[1],
                                                          resources[2],
                                                          resources[3],
                                                          resources[4],
                                                          resources[5],
                                                          resources[6]}});
}
}  // namespace zappy_gui::net
