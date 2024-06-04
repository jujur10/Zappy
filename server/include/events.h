/*
** EPITECH PROJECT, 2024
** events.h
** File description:
** events.h.
*/

#pragma once

#include <stdint.h>

#include "server.h"

/// @brief Function which is used to handle new connections.
/// @param server The server structure.
void on_connection(server_t PTR server);
