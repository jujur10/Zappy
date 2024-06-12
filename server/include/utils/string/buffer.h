/*
** EPITECH PROJECT, 2024
** buffer.h
** File description:
** buffer.h.
*/

#pragma once

#include <stdint.h>

#include "style/status.h"

/// @brief Structure representing buffers.
///
/// @var ptr The ptr on the buffer data.
/// @var len The len of the string.
typedef struct buffer_s {
    char ARRAY ptr;
    uint32_t len;
    uint32_t max_size;
} buffer_t;

/// @brief Function which initializes a new string from a existing string.
///
/// @param buffer The string to init.
/// @param max_size The max size of the buffer.
void init_buffer(buffer_t PTR buffer, uint32_t max_size);

/// @brief Function which initializes a new buffer from a buffer.
/// There is no verification of max_size < len.
///
/// @param buffer The buffer to init (non terminated by '\0').
/// @param max_size The max size of the buffer.
/// @param from The buffer to copy.
/// @param len The buffer length.
void init_buffer_from_chars(buffer_t PTR buffer, uint32_t max_size,
    const char ARRAY from, uint32_t len);

/// @brief Function which sets a new buffer from a buffer.
///
/// @param buffer The buffer to set (non terminated by '\0').
/// @param from The buffer to copy.
/// @param len The new buffer length (should be < to max_size).
void reassign_buffer_from_chars(buffer_t PTR buffer, const char ARRAY from,
    uint32_t len);

/// @brief Function which destroy an existing buffer.
///
/// @param buffer The buffer to clear.
void destroy_buffer(buffer_t PTR buffer);
