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
    int playerIndex;
    int x, y;
    int orientation;
    int level;
};

struct PlayerPositionCommand {
    int playerIndex;
    int x, y;
    int orientation;
};

// ... define other command types ...

// Command variant
using Command = std::variant<PlayerNewCommand, PlayerPositionCommand>;

// Command queue
using Queue = atomic_queue::AtomicQueueB2<Command>;
extern Queue guiToServerQueue;
extern Queue serverToGuiQueue;

}