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
    resources_t total_resources;
    resources_t ARRAY tiles;
    bool has_been_modified;
} map_t;

/// @brief Function which initializes map.
/// @param args The parsed program parameters.
/// @param map The map to initialize.
/// @return 0 on success, 1 on failure.
uint8_t init_map(const argument_t PTR args, map_t PTR map);
