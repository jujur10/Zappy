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
#include "events/new_client_events.h"

/// @brief Structure representing the implies events (link to the msg_t
/// structure)
///
/// @var player_event The player event.
/// @var gui_event The gui event.
typedef struct event_s {
    player_event_t player_event;
    gui_event_t gui_event;
    new_client_event_t new_client_event;
} event_t;
