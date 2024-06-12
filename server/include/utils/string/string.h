/*
** EPITECH PROJECT, 2024
** string.h
** File description:
** string.h.
*/

#pragma once

#include <stdint.h>

#include "style/status.h"

/// @brief Structure representing strings.
///
/// @var ptr The ptr on the string data.
/// @var len The len of the string.
typedef struct string_s {
    char ARRAY ptr;
    uint32_t len;
} string_t;

/// @brief Function which initializes a new string from a existing string.
///
/// @param str The string to init.
/// @param from The string to copy.
void init_string(string_t PTR str, const string_t PTR from);

/// @brief Function which initializes a new string from a buffer.
///
/// @param str The string to init (non terminated by '\0').
/// @param from The buffer to copy.
/// @param len The buffer length.
void init_string_from_chars(string_t PTR str, const char ARRAY from,
    uint32_t len);

/// @brief Function which sets a new string from a buffer.
///
/// @param str The string to set (non terminated by '\0').
/// @param from The buffer to copy.
/// @param len The buffer length.
void reassign_string_from_chars(string_t PTR str, const char ARRAY from,
    uint32_t len);

/// @brief Function which append to the string from a buffer.
///
/// @param str The string to appends to (non terminated by '\0').
/// @param from The buffer to copy.
/// @param len The buffer length.
status_t append_to_string_from_chars(string_t PTR str, const char ARRAY from,
    uint32_t len);

/// @brief Function which clear a existing string (and free).
///
/// @param str The string to clear.
void clear_string(string_t PTR str);
