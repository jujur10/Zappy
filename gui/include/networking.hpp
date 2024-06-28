//
// Created by quentinf on 10/06/24.
//

#pragma once

#include <stop_token>

#include "sockets.hpp"

namespace zappy_gui::net
{
/**
 * @brief Network thread main loop
 *
 * @param stoken Thread stop token
 * @param serverSocket Server socket
 * @param handShakeBuffer The rest of the data read from the server during handshake
 */
void NetworkTreadLoop(const std::stop_token& stoken, const Socket& serverSocket, std::vector<char> handShakeBuffer);
}  // namespace zappy_gui::net
