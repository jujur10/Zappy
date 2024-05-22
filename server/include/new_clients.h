/*
** EPITECH PROJECT, 2024
** new_clients.h
** File description:
** new_clients.h.
*/

#pragma once

#include <stdint.h>
#include <time.h>

/// @brief Structure representing a new client.
///
/// @var sock The client's socket.
/// @var expiration The client socket expiration.
typedef struct new_client_s {
    int sock;
    time_t expiration;
} new_client_t;
