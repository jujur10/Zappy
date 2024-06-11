//
// Created by quentinf on 10/06/24.
//

#pragma once
#include "atomic_queue.h"

namespace zappy_gui::net
{
// Command queue
using GTSQueue = atomic_queue::AtomicQueueB2<char*>;
extern GTSQueue GuiToServerQueue;

}