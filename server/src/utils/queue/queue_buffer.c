/*
** EPITECH PROJECT, 2024
** queue_buffer.c
** File description:
** queue_buffer.c.
*/
#include "utils/queue/queue.h"

void queue_push_buffer_to_queue(queue_t PTR msg_queue,
    const buffer_t PTR buffer)
{
    msg_t message = {};

    create_message_from_buffer(buffer, &message);
    queue_push(msg_queue, &message);
}
