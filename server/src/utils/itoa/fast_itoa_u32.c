/*
** EPITECH PROJECT, 2024
** fast_itoa_u16.c
** File description:
** Andrei Alexandrescu implementation of atoi.
*/
#include <stdint.h>
#include <string.h>

/// @brief Lookup table for converting 2-digit integers into strings.
static const char radix_100_table[] = {
    '0', '0', '0', '1', '0', '2', '0', '3', '0', '4',
    '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
    '1', '0', '1', '1', '1', '2', '1', '3', '1', '4',
    '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
    '2', '0', '2', '1', '2', '2', '2', '3', '2', '4',
    '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
    '3', '0', '3', '1', '3', '2', '3', '3', '3', '4',
    '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
    '4', '0', '4', '1', '4', '2', '4', '3', '4', '4',
    '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
    '5', '0', '5', '1', '5', '2', '5', '3', '5', '4',
    '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
    '6', '0', '6', '1', '6', '2', '6', '3', '6', '4',
    '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
    '7', '0', '7', '1', '7', '2', '7', '3', '7', '4',
    '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
    '8', '0', '8', '1', '8', '2', '8', '3', '8', '4',
    '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
    '9', '0', '9', '1', '9', '2', '9', '3', '9', '4',
    '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
};

uint64_t fast_itoa_u32(uint32_t value, char ARRAY buffer)
{
    char temp[8];
    char *ptr = temp + sizeof(temp);
    const char *buffer_start = buffer;
    uint64_t remaining_length;

    while (value >= 100) {
        ptr -= 2;
        memcpy(ptr, radix_100_table + (value % 100) * 2, 2);
        value /= 100;
    }
    if (value >= 10) {
        memcpy(buffer, radix_100_table + value * 2, 2);
        buffer += 2;
    } else {
        buffer[0] = (char)('0' + value);
        buffer += 1;
    }
    remaining_length = temp + sizeof(temp) - ptr;
    memcpy(buffer, ptr, remaining_length);
    return (buffer + remaining_length) - buffer_start;
}
