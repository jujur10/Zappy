/*
** EPITECH PROJECT, 2024
** swap_clients.h
** File description:
** swap_clients.h.
*/

#pragma once

#include "server.h"

/// @brief Function which transform a new client to a gui.
/// @param server The server pointer.
/// @param client_idx The client's index to transform to gui.
/// @return Success if enough slot for gui available, Failure if not.
status_t swap_new_client_to_gui(server_t *server, uint32_t client_idx);
