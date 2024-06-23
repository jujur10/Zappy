/*
** EPITECH PROJECT, 2024
** arrays_virtual.c
** File description:
** arrays_virtual.c.
*/
#include "utils/arrays/arrays_virtual.h"

uint32_t get_real_index(const virtual_array_t PTR virtual_array,
    uint32_t virtual_index)
{
    uint32_t virtual_x = virtual_index % virtual_array->virtual_width;
    uint32_t virtual_y = virtual_index / virtual_array->virtual_width;
    uint32_t real_x = (virtual_x + virtual_array->virtual_width_off) %
        virtual_array->original_width;
    uint32_t real_y = (virtual_y + virtual_array->virtual_height_off) %
        virtual_array->original_height;
    uint32_t real_index = real_y * virtual_array->original_width + real_x;

    return real_index;
}

void set_offsets(virtual_array_t *virtual_array, int32_t width_offset,
    int32_t height_offset)
{
    virtual_array->virtual_width_off = (int32_t)(width_offset
        + virtual_array->original_width) %
        (int32_t)virtual_array->original_width;
    virtual_array->virtual_height_off = (int32_t)(height_offset
        + virtual_array->original_height) %
        (int32_t)virtual_array->original_height;
}
