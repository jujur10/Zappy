/*
** EPITECH PROJECT, 2024
** ai_handling.h
** File description:
** ai_handling.h.
*/

#pragma once

#include "server.h"

/// @brief Function which initializes an ai (players).
/// @param server The server pointer.
/// @param sock The client's socket.
/// @return The ai index in the ai array of -1 if not enough slot.
int32_t init_ai(server_t PTR server, int sock);

/// @brief Function which destroys an ai.
/// @param server The server pointer.
/// @param ai_idx The ai's index of the ai we want to destroy.
/// @param sock The ai's socket.
void destroy_ai(server_t PTR server, uint32_t ai_idx);

/// @brief Function called when a player is communicating with the server.
/// @param server The server pointer.
/// @param rfds The pointer on the read fd set.
/// @param wfds The pointer on the write fd set.
/// @param select_ret The return value of select.
void handle_players(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret);
