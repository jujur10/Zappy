//
// Created by quentinf on 10/06/24.
//

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#include "server_to_gui_cmd_structs.hpp"
#include "string_utils.hpp"

namespace zappy_gui::net
{
void ParseTileUpdateCommand(const std::string_view& line)
{
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t resources[7] = {0};
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
    ServerToGuiQueue.try_push(UpdateTileCommand{
        .x = x, .y = y, .resources = {resources[0], resources[1], resources[2], resources[3], resources[4], resources[5], resources[6]}});
}

void ParseNewPlayerCommand(const std::string_view& line)
{
    uint16_t id = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t orientation = 0;
    uint8_t level = 0;
    size_t paramPos = 0;

    string_utils::convertFromString(line, id);

    paramPos = line.find_first_of(' ') + 1;
    string_utils::convertFromString(line.data() + paramPos, x);

    paramPos = line.find_first_of(' ', paramPos) + 1;
    string_utils::convertFromString(line.data() + paramPos, y);

    paramPos = line.find_first_of(' ', paramPos) + 1;
    string_utils::convertFromString(line.data() + paramPos, orientation);

    paramPos = line.find_first_of(' ', paramPos) + 1;
    string_utils::convertFromString(line.data() + paramPos, level);

    paramPos = line.find_first_of(' ', paramPos) + 1;

    auto teamName = std::make_unique<char[]>(line.size() - paramPos);
    line.copy(teamName.get(), line.size() - paramPos, paramPos);

    ServerToGuiQueue.try_push(NewPlayerCommand{
        .id = id, .x = x, .y = y, .orientation = orientation, .level = level, .teamName = std::move(teamName)});
}

void ParseDeathOfPlayerCommand(const std::string_view& line)
{
    uint16_t id = 0;

    string_utils::convertFromString(line, id);

    ServerToGuiQueue.try_push(DeadPlayerCommand{.id = id});
}

void ParsePlayerPositionCommand(const std::string_view& line)
{
    uint16_t id = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t orientation = 0;
    size_t paramPos = 0;

    string_utils::convertFromString(line, id);

    paramPos = line.find_first_of(' ') + 1;
    string_utils::convertFromString(line.data() + paramPos, x);

    paramPos = line.find_first_of(' ', paramPos) + 1;
    string_utils::convertFromString(line.data() + paramPos, y);

    paramPos = line.find_first_of(' ', paramPos) + 1;
    string_utils::convertFromString(line.data() + paramPos, orientation);

    ServerToGuiQueue.try_push(PlayerPositionCommand{.id = id, .x = x, .y = y, .orientation = orientation});
}

void ParseTimeUnitUpdatedCommand(const std::string_view& line)
{
    uint32_t timeUnit = 0;

    string_utils::convertFromString(line, timeUnit);

    ServerToGuiQueue.try_push(TimeUnitUpdateCommand{.timeUnit = timeUnit});
}

void ParseStartIncantationCommand(const std::string_view& line)
{
    uint16_t x = 0;
    uint16_t y = 0;
    auto playerIdArray = std::vector<uint16_t>();
    playerIdArray.reserve(8);
    size_t paramPos = 0;

    string_utils::convertFromString(line, x);

    paramPos = line.find(' ') + 1;
    string_utils::convertFromString(line.data() + paramPos, y);

    paramPos = line.find(' ', paramPos) + 1;

    uint16_t i = 0;
    for (; true; ++i)
    {
        uint16_t playerId = 0;
        if (!string_utils::convertFromString(line.data() + paramPos, playerId))
        {
            break;
        }
        playerIdArray.emplace_back(playerId);
        paramPos = line.find(' ', paramPos) + 1;
        if (0 == paramPos)
        {
            break;
        }
    }

    auto playerIds = std::make_unique<uint16_t[]>(playerIdArray.size());
    std::ranges::copy(playerIdArray, playerIds.get());

    ServerToGuiQueue.try_push(StartIncantationCommand{
        .x = x, .y = y, .playerCount = i, .playerIds = std::move(playerIds)});
}

void ParseEndIncantationCommand(const std::string_view& line)
{
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t success = false;
    size_t paramPos = 0;

    string_utils::convertFromString(line, x);

    paramPos = line.find(' ') + 1;

    string_utils::convertFromString(line.data() + paramPos, y);

    paramPos = line.find(' ', paramPos) + 1;

    string_utils::convertFromString(line.data() + paramPos, success);

    ServerToGuiQueue.try_push(EndIncantationCommand{
        .x = x, .y = y, .success = static_cast<bool>(success)});
}

void ParsePlayerInventoryCommand(const std::string_view& line)
{
    uint16_t id = 0;
    uint16_t resources[7] = {0};
    size_t paramPos = 0;

    string_utils::convertFromString(line, id);

    paramPos = line.find(' ') + 1;
    paramPos = line.find(' ', paramPos) + 1; // Skip the "x" field
    paramPos = line.find(' ', paramPos) + 1; // Skip the "y" field

    for (uint16_t& resource : resources)
    {
        string_utils::convertFromString(line.data() + paramPos, resource);
        paramPos = line.find(' ', paramPos) + 1;
    }
    ServerToGuiQueue.try_push(PlayerInventoryCommand{
        .id = id, .resources = {resources[0], resources[1], resources[2], resources[3], resources[4], resources[5], resources[6]}});
}

void ParsePlayerPickupCommand(const std::string_view& line)
{
    uint16_t id = 0;
    uint16_t resource = 0;

    string_utils::convertFromString(line, id);

    const size_t paramPos = line.find(' ') + 1;
    if (paramPos <= 1)
    {
        return;
    }
    if (!string_utils::convertFromString(line.data() + paramPos, resource) || resource > 6)
    {
        return;
    }

    ServerToGuiQueue.try_push(PlayerPickupCommand{.id = id, .resource = resource});
}

void ParsePlayerDropCommand(const std::string_view& line)
{
    uint16_t id = 0;
    uint16_t resource = 0;

    string_utils::convertFromString(line, id);

    const size_t paramPos = line.find(' ') + 1;
    if (paramPos <= 1)
    {
        return;
    }
    if (!string_utils::convertFromString(line.data() + paramPos, resource) || resource > 6)
    {
        return;
    }
    ServerToGuiQueue.try_push(PlayerDropCommand{.id = id, .resource = resource});
}

void ParseTeamNameCommand(const std::string_view& line)
{
    auto teamName = std::make_unique<char[]>(line.size());
    std::ranges::copy(line, teamName.get());
    ServerToGuiQueue.try_push(TeamNameCommand{.teamName = std::move(teamName)});
}
}  // namespace zappy_gui::net
