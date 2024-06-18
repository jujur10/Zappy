/*
** EPITECH PROJECT, 2024
** coordinates.h
** File description:
** coordinates.h.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "style/macros.h"

/// @brief Macro used to make a result absolute.
#define ABSOLUTE(result) (((result) < 0) ? -(result) : (result))

/// @brief Macro used to represent a full circle in radian.
#define FULL_CIRCLE (2 * M_PI)

#define MIN(x, y) ((x) < (y)) ? (x) : (y)

/// @brief Macro used to normalize a radian on [0; 2π].
#define NORMALIZE_RAD(rad) (fmod((rad) + FULL_CIRCLE, FULL_CIRCLE))

/// @brief Macro to convert a normalized radian value to degree.
#define RADIAN_TO_DEGREE(x) ((x) * (180.0 / M_PI))

/// @brief Structure representing coordinates.
///
/// @var x The vertical position.
/// @var y The horizontal position.
typedef struct coordinates_s {
    uint16_t x;
    uint16_t y;
} coordinates_t;

/// @brief Structure representing positive and negative coordinates.
///
/// @var x The vertical position.
/// @var y The horizontal position.
typedef struct coordinates_i32_s {
    int32_t x;
    int32_t y;
} coordinates_i32_t;

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

/// @brief Function which calculates the distance between 2 points using
/// Pythagoras theorem.
///
/// @param coordinates_1 The first coordinate.
/// @param coordinates_2 The second coordinate.
/// @return The distance between theses points.
double get_distance(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2);

/// @brief Function which calculates the distance between 2 points.
///
/// @param coordinates_1 The first coordinate.
/// @param coordinates_2 The second coordinate.
/// @param coordinates_output The calculated distance.
void get_vec_distance(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2,
    coordinates_i32_t PTR coordinates_output);

/// @brief Function which returns the angle between two coordinates.
///
/// @param coordinates_1 The first coordinate.
/// @param coordinates_2 The second coordinate.
/// @return The angle in normalized radian [0; 2π].
double get_rad_angle(const coordinates_t PTR coordinates_1,
    const coordinates_t PTR coordinates_2);
