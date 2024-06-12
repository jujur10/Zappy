/*
** EPITECH PROJECT, 2024
** buffer.c
** File description:
** buffer.c.
*/
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "utils/string/buffer.h"

void init_buffer(buffer_t *buffer, uint32_t max_size)
{
    buffer->ptr = malloc(sizeof(char) * (max_size + 1));
    buffer->max_size = max_size;
}

void init_buffer_from_chars(buffer_t *buffer, uint32_t max_size,
    const char *from, uint32_t len)
{
    buffer->ptr = malloc(sizeof(char) * (max_size + 1));
    memcpy(buffer->ptr, from, len);
    buffer->ptr[len] = '\0';
    buffer->len = len;
    buffer->max_size = max_size;
}

void reassign_buffer_from_chars(buffer_t *buffer, const char *from,
    uint32_t len)
{
    free(buffer->ptr);
    buffer->ptr = malloc(sizeof(char) * (len + 1));
    buffer->len = len;
    memcpy(buffer->ptr, from, len);
    buffer->ptr[len] = '\0';
}

void destroy_buffer(buffer_t *buffer)
{
    free(buffer->ptr);
    buffer->ptr = NULL;
    buffer->len = 0;
    buffer->max_size = 0;
}
