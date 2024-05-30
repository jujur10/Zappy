/*
** EPITECH PROJECT, 2024
** queue.h
** File description:
** queue.h.
*/

#pragma once

#include <stdint.h>
#include <sys/queue.h>

#include "shared_msg.h"

/// @brief Structure representing shared message container.
///
/// @var message The shared message.
/// @var entries The queue entries.
typedef struct shared_message_container_s {
    shared_msg_t message;
    TAILQ_ENTRY(shared_message_container_s) entries;
} shared_message_container_t;

typedef struct shared_message_queue_head_s shared_message_queue_head_t;

TAILQ_HEAD(shared_message_queue_head_s, shared_message_container_t);
