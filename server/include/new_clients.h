/*
** EPITECH PROJECT, 2024
** new_clients.h
** File description:
** new_clients.h.
*/

#pragma once

#include <stdint.h>
#include <time.h>

#include "utils/queue/queue.h"

/// @brief Structure representing a new client.
///
/// @var sock The client's socket.
/// @var expiration The client's socket expiration.
typedef struct new_client_s {
    int sock;
    struct timespec expiration;
    queue_t queue;
} new_client_t;
