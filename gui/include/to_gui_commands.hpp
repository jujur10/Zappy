//
// Created by quentinf on 10/06/24.
//

#pragma once
#include <variant>
#include "atomic_queue.h"

namespace zappy_gui::net
{
// Command types
struct UpdateTileCommand {
    uint16_t x;
    uint16_t y;
    uint16_t resources[7];
};

// ... define other command types ...

// Command variant
// This is the type that will be used to store the commands in the queue
// DON'T CHANGE THE ORDER OF THE TYPES IN THE VARIANT OR YOU WILL BREAK THE PARSING CODE
using Command = std::variant<UpdateTileCommand, std::monostate>;

// Command queue
using STGQueue = atomic_queue::AtomicQueueB2<Command>;
extern STGQueue ServerToGuiQueue;

// Parsing functions for commands coming from the server

void ParseTileUpdateCommand(const std::string_view& line);

}