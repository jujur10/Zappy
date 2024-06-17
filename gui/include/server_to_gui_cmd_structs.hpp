//
// Created by quentinf on 10/06/24.
//

#pragma once
#include <variant>

#include "atomic_queue.h"

namespace zappy_gui::net
{
/// @brief bct command representation
struct UpdateTileCommand
{
    uint16_t x;
    uint16_t y;
    uint16_t resources[7];
};

struct NewPlayerCommand
{
    uint16_t id;
    uint16_t x;
    uint16_t y;
    uint8_t orientation;
    uint8_t level;
    const char *teamName;
};

struct DeadPlayerCommand
{
    uint16_t id;
};

// ... define other command types ...

/// @brief Command variant type that will be used to store the different commands in the queue
// DON'T CHANGE THE ORDER OF THE TYPES IN THE VARIANT OR YOU WILL BREAK THE PARSING CODE
using Command = std::variant<UpdateTileCommand, NewPlayerCommand, DeadPlayerCommand>;


// Command queue server -> gui
using STGQueue = atomic_queue::AtomicQueueB2<Command>;
/// @var Thread-safe queue to send commands from the server to the GUI
extern STGQueue ServerToGuiQueue;



// Parsing functions for commands coming from the server

/// @brief Parse the bct command received as text from the server.
/// This function will parse the bct command and push the corresponding UpdateTileCommand in the ServerToGuiQueue
/// @param line The bct command received from the server
void ParseTileUpdateCommand(const std::string_view& line);

/// @brief Parse the pnw command received as text from the server.
/// This function will parse the pnw command and push the corresponding NewPlayerCommand in the ServerToGuiQueue
/// @param line The pnw command received from the server
void ParseNewPlayerCommand(const std::string_view& line);
}  // namespace zappy_gui::net
