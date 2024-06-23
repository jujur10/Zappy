/*
** EPITECH PROJECT, 2024
** arrays_virtual.h
** File description:
** arrays_virtual.h.
*/

#pragma once

#include <stdint.h>

typedef struct virtual_array_s {
    uint32_t original_width;
    uint32_t original_height;
    uint32_t virtual_width;
    uint32_t virtual_height;
    uint32_t virtual_width_off;
    uint32_t virtual_height_off;
} virtual_array_t;

/// @brief Function to obtain real index from virtual index.
///
/// @param virtual_array The virtual array we are working on.
/// @param virtual_index The virtual index.
/// @return The real index into the real array.
uint32_t get_real_index(const virtual_array_t PTR virtual_array,
    uint32_t virtual_index);

/// @brief Function which sets the offset to the virtual array.
///
/// @param virtual_array The virtual array we are working on.
/// @param width_offset The width offset.
/// @param height_offset The height offset.
void set_offsets(virtual_array_t *virtual_array, int32_t width_offset,
    int32_t height_offset);
