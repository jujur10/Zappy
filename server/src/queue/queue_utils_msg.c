/*
** EPITECH PROJECT, 2024
** queue_utils.c
** File description:
** queue_utils.c.
*/
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "style/status.h"
#include "queue/msg_queue.h"
#include "msg.h"

status_t add_msg_to_queue(msg_queue_head_t PTR msg_queue,
    const msg_t PTR message)
{
    msg_container_t *msg_ctn = malloc(sizeof(msg_container_t));

    if (NULL == msg_ctn)
        return FAILURE;
    msg_ctn->message = *message;
    TAILQ_INSERT_TAIL(msg_queue, msg_ctn, entries);
    return SUCCESS;
}

status_t pop_msg(msg_queue_head_t PTR msg_queue, msg_t PTR message)
{
    msg_container_t *msg_ctn = TAILQ_FIRST(msg_queue);

    if (NULL == msg_ctn)
        return FAILURE;
    if (message != NULL)
        *message = msg_ctn->message;
    TAILQ_REMOVE(msg_queue, msg_ctn, entries);
    free(msg_ctn);
    return SUCCESS;
}

void clear_msg_queue(msg_queue_head_t PTR queue)
{
    msg_container_t *current_element = TAILQ_FIRST(queue);
    msg_container_t *temp;

    while (current_element != NULL) {
        temp = TAILQ_NEXT(current_element, entries);
        destroy_message(&current_element->message);
        free(current_element);
        current_element = temp;
    }
}
