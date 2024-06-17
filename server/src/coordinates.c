/*
** EPITECH PROJECT, 2024
** coordinates.c
** File description:
** coordinates.c.
*/
#include <stdbool.h>
#include <string.h>

#include "coordinates.h"

bool is_coordinates_equal(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2)
{
    return 0 == memcmp(coordinates_1, coordinates_2, sizeof(coordinates_t));
}
