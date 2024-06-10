//
// Created by quentinf on 10/06/24.
//

#pragma once

#include <stop_token>

#include "sockets.hpp"

namespace zappy_gui::net
{
void networkTreadLoop(const std::stop_token& stoken,
                      const Socket& serverSocket);
}