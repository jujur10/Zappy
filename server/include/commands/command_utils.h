/*
** EPITECH PROJECT, 2024
** command_utils.h
** File description:
** command_utils.h.
*/

#pragma once

#include <stdint.h>

/// @brief Write a number to the buffer and increment the count value.
///
/// param nb The number to write into the buffer.
/// param buffer The buffer to modify.
/// param count The count value to use and modify.
void write_nb_to_buffer(uint32_t nb, char ARRAY buffer,
    uint32_t PTR count);
