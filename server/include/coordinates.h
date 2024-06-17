/*
** EPITECH PROJECT, 2024
** coordinates.h
** File description:
** coordinates.h.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "style/macros.h"

/// @brief Structure representing coordinates.
///
/// @var x The vertical position.
/// @var y The horizontal position.
typedef struct coordinates_s {
    uint16_t x;
    uint16_t y;
} coordinates_t;

/// @brief Enumeration representing orientation.
///
/// @var LOOK_TOP Representation of look up.
/// @var LOOK_RIGHT Representation of look right.
/// @var LOOK_BOTTOM Representation of look bottom.
/// @var LOOK_LEFT Representation of look left.
typedef enum orientation_s {
    LOOK_TOP,
    LOOK_RIGHT,
    LOOK_BOTTOM,
    LOOK_LEFT,
    NB_OF_ORIENTATION
} PACKED orientation_t;

/// @brief Function which verify if the coordinates are equal or not.
///
/// @param coordinates_1 The first coordinates to verify.
/// @param coordinates_2 The second coordinates to verify.
/// @return True if coordinates equal, False if not.
bool is_coordinates_equal(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2);
