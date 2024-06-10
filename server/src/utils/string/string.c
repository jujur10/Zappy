/*
** EPITECH PROJECT, 2024
** string.c
** File description:
** string.c.
*/
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "style/status.h"
#include "utils/string/string.h"

void init_string(string_t *str, const string_t *from)
{
    str->ptr = malloc(sizeof(char) * (from->len + 1));
    memmove(str->ptr, from->ptr, from->len);
    str->ptr[from->len] = '\0';
    str->len = from->len;
}

void init_string_from_char(string_t *str, const char *from, uint32_t len)
{
    str->ptr = malloc(sizeof(char) * (len + 1));
    memmove(str->ptr, from, len);
    str->ptr[len] = '\0';
    str->len = len;
}

void set_string_from_char(string_t *str, const char *from, uint32_t len)
{
    free(str->ptr);
    str->ptr = malloc(sizeof(char) * (len + 1));
    str->len = len;
    memmove(str->ptr, from, len);
    str->ptr[len] = '\0';
}

status_t append_to_string_from_char(string_t *str, const char *from,
    uint32_t len)
{
    char *temp_ptr = realloc(str->ptr, str->len + len + 1);

    if (NULL == temp_ptr)
        return FAILURE;
    memcpy(temp_ptr + str->len, from, len);
    temp_ptr[str->len + len] = '\0';
    str->ptr = temp_ptr;
    str->len = str->len + len;
    return SUCCESS;
}

void clear_string(string_t *str)
{
    free(str->ptr);
    str->ptr = NULL;
    str->len = 0;
}
