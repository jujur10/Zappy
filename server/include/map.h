/*
** EPITECH PROJECT, 2024
** map.h
** File description:
** map.h.
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "resources.h"
#include "style/status.h"

/// @brief Structure representing player.
///
/// @var height The map height.
/// @var width The map width.
/// @var total_resources The total resources of the map.
/// @var tiles The map representation.
/// @var has_been_modified Is the map has been modified.
typedef struct map_s {
    uint32_t height;
    uint32_t width;
    resources64_t total_resources;
    resources_t ARRAY tiles;
    bool has_been_modified;
} map_t;

/// @brief Function which initializes map.
/// @param args The parsed program parameters.
/// @param map The map to initialize.
/// @return SUCCESS or FAILURE.
status_t init_map(const argument_t PTR args, map_t PTR map);

/// @brief Function which spread resources on the map.
/// @param map The map to modify.
void spread_resources_on_map(map_t PTR map);

/// @brief Function which check if update available and update if available.
/// @param current_time The current in-game time (time_units attribute of
/// server)
/// @param map The map to modify.
void update_map(double current_time, map_t PTR map);
