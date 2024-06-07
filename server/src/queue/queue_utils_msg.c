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
#include "logging.h"

status_t add_msg_to_queue(msg_queue_head_t PTR msg_queue,
    const msg_t PTR message)
{
    msg_container_t *msg_ctn = calloc(1, sizeof(msg_container_t));

    if (NULL == msg_ctn) {
        ERROR("Message container allocation failed")
        return FAILURE;
    }
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
    if (TAILQ_FIRST(msg_queue) == msg_ctn) {
        msg_queue->tqh_first = NULL;
        msg_queue->tqh_last = &msg_queue->tqh_first;
    }
    free(msg_ctn);
    return SUCCESS;
}

status_t queue_empty(const msg_queue_head_t PTR msg_queue)
{
    if (NULL == msg_queue->tqh_first)
        return SUCCESS;
    return FAILURE;
}

void clear_msg_queue(msg_queue_head_t PTR queue)
{
    msg_container_t *current_element = TAILQ_FIRST(queue);
    msg_container_t *temp;

    LOGF("Clearing %lu queue", (uint64_t)queue);
    while (current_element != NULL) {
        temp = TAILQ_NEXT(current_element, entries);
        destroy_message(&current_element->message);
        free(current_element);
        current_element = temp;
    }
}
