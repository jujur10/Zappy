/*
** EPITECH PROJECT, 2024
** queue.c
** File description:
** queue.c.
*/
#include <stdlib.h>
#include <string.h>

#include "utils/queue/queue.h"

void queue_new(queue_t PTR queue)
{
    queue->data = malloc(sizeof(msg_t));
    queue->head = 0;
    queue->tail = 0;
    queue->len = 0;
    queue->nb_of_allocated_elements = 1;
}

void queue_destroy(queue_t PTR queue)
{
    msg_t message;

    while (SUCCESS == queue_pop(queue, &message))
        destroy_message(&message);
    free(queue->data);
}

status_t queue_push(queue_t PTR queue, const msg_t PTR msg)
{
    msg_t *temp_ptr;

    if (queue->len == queue->nb_of_allocated_elements) {
        queue->nb_of_allocated_elements <<= 1;
        temp_ptr = realloc(queue->data,
            queue->nb_of_allocated_elements * sizeof(msg_t));
        if (NULL == temp_ptr)
            return FAILURE;
        queue->data = temp_ptr;
        memcpy(queue->data + queue->len,
            queue->data,
            queue->head * sizeof(msg_t));
        queue->head += queue->len;
    }
    queue->data[queue->head] = *msg;
    queue->head = (queue->head + 1) & (queue->nb_of_allocated_elements - 1);
    queue->len++;
    return SUCCESS;
}

status_t queue_empty(const queue_t PTR queue)
{
    if (0 == queue->len)
        return SUCCESS;
    return FAILURE;
}

status_t queue_pop(queue_t PTR queue, msg_t PTR message)
{
    if (0 == queue->len)
        return FAILURE;
    *message = queue->data[queue->tail];
    queue->tail = (queue->tail + 1) & (queue->nb_of_allocated_elements - 1);
    --queue->len;
    return SUCCESS;
}
