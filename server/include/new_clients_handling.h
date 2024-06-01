/*
** EPITECH PROJECT, 2024
** new_clients_handling.h
** File description:
** new_clients_handling.h.
*/

#pragma once

#include "server.h"

/// @brief Function which initialize a client.\n
/// - Send "WELCOME\\n" to the client.
/// - Add the timeout to the client's clock.
/// @param server The server pointer.
/// @param client The client pointer we want to initialize.
void init_new_client(server_t PTR server, new_client_t PTR client);

/// @brief Function which destroy a client.
/// @param server The server pointer.
/// @param client The client pointer we want to destroy.
void destroy_new_client(server_t PTR server, new_client_t PTR client);

/// @brief Function called when the server receive data from a new_client_t.\n
/// - Put the client's message into a buffer.\n
/// - If EOF reached, destroy the client.\n
/// - Else, find out if the requested team exists.\n
/// - If the team doesn't exist, destroy the client.\n
/// - If the team exists, respond "ok\\n" to the client.
/// @param server The server pointer.
/// @param client The pointer to the client who sent the message.
void on_new_client_rcv(server_t PTR server, new_client_t PTR client);

/// @brief Function called when a new_client_t is communicating with the
/// server.\n
/// @param server The server pointer.
/// @param client The pointer to the client who sent the message.
void handle_new_clients(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret);
