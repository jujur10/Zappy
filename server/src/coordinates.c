/*
** EPITECH PROJECT, 2024
** coordinates.c
** File description:
** coordinates.c.
*/
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "coordinates.h"

bool is_coordinates_equal(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2)
{
    return 0 == memcmp(coordinates_1, coordinates_2, sizeof(coordinates_t));
}

double get_distance(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2)
{
    uint32_t x_delta = ABSOLUTE(coordinates_2->x - coordinates_1->x);
    uint32_t y_delta = ABSOLUTE(coordinates_2->y - coordinates_1->y);
    uint32_t a_square = x_delta * x_delta;
    uint32_t b_square = y_delta * y_delta;
    uint32_t c_square = a_square + b_square;

    return sqrt(c_square);
}

void get_vec_distance(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2,
    coordinates_i32_t PTR coordinates_output)
{
    coordinates_output->x = coordinates_2->x - coordinates_1->x;
    coordinates_output->y = coordinates_2->y - coordinates_1->y;
}

double get_rad_angle(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2)
{
    double x_delta = coordinates_2->x - coordinates_1->x;
    double y_delta = coordinates_2->y - coordinates_1->y;

    return NORMALIZE_RAD(-atan2(y_delta, x_delta));
}
