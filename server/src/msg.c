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

status_t create_message(const char ARRAY msg_content, uint32_t len,
    msg_t PTR message)
{
    message->ptr = malloc(sizeof(char) * len);
    message->len = len;
    if (NULL == message->ptr)
        return FAILURE;
    memcpy(message->ptr, msg_content, len);
    return SUCCESS;
}

void destroy_message(msg_t PTR message)
{
    free(message->ptr);
}
