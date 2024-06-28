/*
** EPITECH PROJECT, 2024
** server_update.h
** File description:
** server_update.h.
*/

#pragma once

#include "server.h"

/// @brief Function used to update game elements before client handling.
///
/// @param server The server structure to update.
void update_server(server_t PTR server);

/// @brief Function used to check if the end of the game happened.
///
/// @param server The server structure.
void check_end_of_the_game(server_t PTR server);
