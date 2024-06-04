/*
** EPITECH PROJECT, 2024
** msg_queue.h
** File description:
** msg_queue.h.
*/

#pragma once

#include <stdint.h>
#include <sys/queue.h>

#include "style/status.h"

#include "msg.h"

/// @brief Structure representing message container.
///
/// @var message The message.
/// @var entries The queue entries.
typedef struct msg_container_s {
    msg_t message;
    TAILQ_ENTRY(msg_container_s) entries;
} msg_container_t;

typedef struct msg_queue_head_s msg_queue_head_t;
TAILQ_HEAD(msg_queue_head_s, msg_container_s);

/// @brief Function which adds a message to a message queue.
///
/// @param msg_queue The message queue.
/// @param message The message to add to queue.
/// @return 0 for success, 1 for failure.
status_t add_msg_to_queue(msg_queue_head_t PTR msg_queue,
    const msg_t PTR message);

/// @brief Function which removes the next message of the message queue and
/// returns it into message.
///
/// @param msg_queue The message queue.
/// @param message The pointer on the message structure to write.
/// @return 0 for success, 1 for failure.
status_t pop_msg(msg_queue_head_t PTR msg_queue, msg_t PTR message);

/// @brief Function that clears a message queue.
///
/// @param queue The message queue.
void clear_msg_queue(msg_queue_head_t PTR queue);
