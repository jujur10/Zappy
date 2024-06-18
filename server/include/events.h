/*
** EPITECH PROJECT, 2024
** events.h
** File description:
** events.h.
*/

#pragma once

#include <stdint.h>

#include "events/player_events.h"
#include "events/gui_events.h"

/// @brief Structure representing the implies events (link to the msg_t
/// structure)
///
/// @var player_event The player event.
/// @var gui_event The gui event.
typedef struct event_s {
    player_event_t player_event;
    gui_event_t gui_event;
} event_t;

/// @brief Function which is used to handle new connections.
/// @param server The server structure.
void on_connection(server_t PTR server);
