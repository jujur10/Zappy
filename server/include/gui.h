/*
** EPITECH PROJECT, 2024
** gui.h
** File description:
** gui.h.
*/

#pragma once

#include <stdint.h>
#include <time.h>

#include "utils/queue/queue.h"
#include "commands/gui_commands.h"
#include "utils/queue/queue.h"

/// @brief Structure representing GUI.
///
/// @var sock The GUI's socket.
/// @var queue The GUI's receiving message queue.
/// @var command_buffer The GUI's command buffer.
typedef struct gui_s {
    int sock;
    queue_t queue;
    gui_command_buffer_t command_buffer;
} gui_t;
