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
