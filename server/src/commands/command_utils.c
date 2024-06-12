/*
** EPITECH PROJECT, 2024
** command_utils.c
** File description:
** command_utils.c.
*/
#include <stdint.h>

#include "utils/itoa/fast_itoa.h"

void write_nb_to_buffer(uint32_t nb, char ARRAY buffer,
    uint32_t PTR count)
{
    (*count) += fast_itoa_u32(nb, buffer + *count);
    buffer[*count] = ' ';
    (*count)++;
}
