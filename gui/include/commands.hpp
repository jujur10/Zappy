//
// Created by quentinf on 10/06/24.
//

#pragma once
#include <variant>
#include "atomic_queue.h"

namespace zappy_gui::net
{
// Command types
struct PlayerNewCommand {
    // example
};

struct PlayerPositionCommand {
    // example
};

// ... define other command types ...

// Command variant
using Command = std::variant<PlayerNewCommand, PlayerPositionCommand>;

// Command queue
using Queue = atomic_queue::AtomicQueueB2<Command>;
extern Queue serverToGuiQueue;

}