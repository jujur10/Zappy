/*
** EPITECH PROJECT, 2024
** gui.h
** File description:
** gui.h.
*/

#pragma once

#include <stdint.h>
#include <time.h>

#include "queue/msg_queue.h"

/// @brief Structure representing GUI.
///
/// @var sock The GUI's socket.
/// @var blocking_time Representing the time at which the GUI can execute an
///  action again.
/// @var queue The GUI's receiving message queue.
typedef struct gui_s {
    int sock;
    struct timespec blocking_time;
    msg_queue_head_t queue;
} gui_t;
