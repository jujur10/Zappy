/*
** EPITECH PROJECT, 2024
** queue.h
** File description:
** queue.h.
*/

#pragma once

#include "msg.h"

/// @brief Structure representing a message queue.
///
/// @var data The message array.
/// @var head The queue head.
/// @var tail The queue tail.
/// @var data The queue length.
/// @var data The queue nb_of_allocated_elements.
typedef struct queue_s {
    msg_t ARRAY data;
    uint32_t head;
    uint32_t tail;
    uint32_t len;
    uint32_t nb_of_allocated_elements;
} queue_t;

/// @brief Function which initializes a queue.
///
/// @param queue The queue to initialize.
void queue_new(queue_t PTR queue);

/// @brief Function which destroys a queue and it's elements.
///
/// @param queue The queue to destroy.
void queue_destroy(queue_t PTR queue);

/// @brief Function which adds a message to a message queue.\n
/// Detailed behavior:\n
/// - Add a message to the queue.\n
/// - Update the head index (bitwise in order to warp around the array).\n
/// - If queue is full, reallocate the queue of the double of his actual\n
///   size using bit shifting.\n
/// - If the allocation fail, return FAILURE.
///
/// @param queue The message queue.
/// @param message The message to add to queue.
status_t queue_push(queue_t PTR queue, const msg_t PTR msg);

/// @brief Function which tests if the queue is empty.
///
/// @param queue The message queue.
/// @return Success if the queue is empty, Failure if not.
status_t queue_empty(const queue_t PTR queue);

/// @brief Function which removes the next message of the message queue and
/// returns it into message.\n
///
/// @param queue The message queue.
/// @param message The pointer on the message structure to write.
/// @return SUCCESS or FAILURE.
status_t queue_pop(queue_t PTR queue, msg_t PTR message);

/// @brief Function which adds a buffer to the queue.
///
/// @param msg_queue The message queue.
/// @param buffer The buffer to push.
void queue_push_buffer_to_queue(queue_t PTR msg_queue,
    const buffer_t PTR buffer);
