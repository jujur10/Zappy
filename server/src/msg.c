/*
** EPITECH PROJECT, 2024
** msg.c
** File description:
** msg.c.
*/
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "style/status.h"
#include "msg.h"
#include "utils/string/buffer.h"

status_t create_message(const char ARRAY msg_content, uint32_t len,
    msg_t PTR message)
{
    message->ptr = malloc(sizeof(char) * len);
    message->len = len;
    if (NULL == message->ptr)
        return FAILURE;
    memcpy(message->ptr, msg_content, len);
    message->to_free = true;
    return SUCCESS;
}

void create_message_from_ptr(char ARRAY ptr, uint32_t len,
    msg_t PTR message)
{
    message->ptr = ptr;
    message->len = len;
    message->to_free = false;
}

void create_message_from_buffer(const buffer_t PTR buffer, msg_t PTR message)
{
    message->ptr = buffer->ptr;
    message->len = buffer->len;
    message->to_free = false;
}

void destroy_message(msg_t PTR message)
{
    if (true == message->to_free)
        free(message->ptr);
}
