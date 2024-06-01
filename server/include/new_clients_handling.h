/*
** EPITECH PROJECT, 2024
** new_clients_handling.h
** File description:
** new_clients_handling.h.
*/

#pragma once

#include "server.h"

/// @brief Function which initialize a client.
/// @param server The server pointer.
/// @param client The client pointer we want to initialize.
void init_new_client(server_t PTR server, new_client_t PTR client);

/// @brief Function which destroy a client.
/// @param server The server pointer.
/// @param client The client pointer we want to destroy.
void destroy_new_client(server_t PTR server, new_client_t PTR client);

/// @brief Function called when a new client received something.
/// @param server The server pointer.
/// @param client The pointer to the client who sent the message.
void on_new_client_rcv(server_t PTR server, new_client_t PTR client);

/// @brief Function called when for the new client handling.
/// @param server The server pointer.
/// @param client The pointer to the client who sent the message.
void handle_new_clients(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret);
