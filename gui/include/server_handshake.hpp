//
// Created by quentinf on 15/06/24.
//

#pragma once
#include <vector>

namespace zappy_gui
{
class Socket;
/**
 * @brief Performs the handshake process with the server.
 *
 * This function handles the handshake process with the server. It exchanges
 * necessary information and validates the received data to establish a
 * connection and retrieve the map dimensions.
 *
 * The function follows these steps:
 * 1. Reads the welcome message from the server and verifies it.
 * 2. Sends the team name to the server.
 * 3. Receives the client number from the server and validates it.
 * 4. Receives the map dimensions (width and height) from the server and
 * validates them.
 * 5. Sets the `map::MAP_WIDTH` and `map::MAP_HEIGHT` variables with the
 * received map dimensions.
 *
 * If any errors occur during the handshake process, such as receiving invalid
 * data or failing to convert the data, the function writes an appropriate error
 * message to the error writer and exits the program with a status code of 1.
 *
 * @param serverSocket The socket used for communication with the server.
 * @return The rest of the data read from the server during the handshake.
 */
std::vector<char> Handshake(const Socket &serverSocket);
}  // namespace zappy_gui
