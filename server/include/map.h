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
#include "utils/pre_generate/pre_generate.h"

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
///
/// @param args The parsed program parameters.
/// @param map The map to initialize.
/// @return SUCCESS or FAILURE.
status_t init_map(const argument_t PTR args, map_t PTR map);

/// @brief Function which spread resources on the map.
///
/// @param map The map to modify.
/// @param generated_buffers The already generated buffers.
void spread_resources_on_map(map_t PTR map,
    const generated_buffers_t PTR generated_buffers);

/// @brief Function which check if update available and update if available.
///
/// @param current_time The current in-game time (time_units attribute of
/// server)
/// @param map The map to modify.
/// @param generated_buffers The already generated buffers.
void update_map(double current_time, map_t PTR map,
    const generated_buffers_t PTR generated_buffers);

/// @brief Function which check if update available and update if available.
///
/// @param map The map.
/// @param buffer The map buffer.
void update_map_buffer(const map_t PTR map, buffer_t PTR buffer);
