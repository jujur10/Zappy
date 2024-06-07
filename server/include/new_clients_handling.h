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

/// @brief Function which destroy a client.\n
/// - Move the last element to the index we want to destroy, and set to zero\n
/// the old position of the last element.
/// @param server The server pointer.
/// @param client_idx The client index we want to destroy.
/// @param preserve_sock true to preserve socket, false to close it.
void destroy_new_client(server_t PTR server, uint32_t client_idx,
    bool preserve_sock);

/// @brief Function called when a new_client_t is communicating with the
/// server.\n
/// @param server The server pointer.
/// @param rfds The pointer on the read fd set.
/// @param wfds The pointer on the write fd set.
/// @param select_ret The return value of select.
void handle_new_clients(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret);
