/*
** EPITECH PROJECT, 2024
** coordinates.h
** File description:
** coordinates.h.
*/

#pragma once

#include <stdint.h>

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Structure representing coordinates.
///
/// @var x The vertical position.
/// @var y The horizontal position.
typedef struct coordinates_s {
    uint32_t x;
    uint32_t y;
} coordinates_t;

/// @brief Enumeration representing orientation.
///
/// @var LOOK_TOP Representation of look up.
/// @var LOOK_BOTTOM Representation of look bottom.
/// @var LOOK_RIGHT Representation of look right.
/// @var LOOK_LEFT Representation of look left.
typedef enum orientation_s {
    LOOK_TOP,
    LOOK_BOTTOM,
    LOOK_RIGHT,
    LOOK_LEFT
} PACKED orientation_t;
