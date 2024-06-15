//
// Created by quentinf on 10/06/24.
//

#pragma once
#include "atomic_queue.h"

namespace zappy_gui::net
{
// Command queue gui -> server
using GTSQueue = atomic_queue::AtomicQueueB2<char*>;
/// @var Thread-safe queue to send commands from the GUI to the server
extern GTSQueue GuiToServerQueue;

}  // namespace zappy_gui::net
