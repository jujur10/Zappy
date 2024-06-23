/*
** EPITECH PROJECT, 2024
** inventory.h
** File description:
** inventory.h.
*/

#pragma once

#include <stdint.h>

#include "style/macros.h"

/// @brief Structure representing the inventory attributes.
///
/// @var food The food counter.
/// @var linemate The Linemate counter.
/// @var deraumere The Deraumere counter.
/// @var sibur The Sibur counter.
/// @var mendiane The Mendiane counter.
/// @var phiras The Phiras counter.
/// @var thystame The Thystame counter.
typedef struct inventory_attr_s {
    uint16_t food;
    uint16_t linemate;
    uint16_t deraumere;
    uint16_t sibur;
    uint16_t mendiane;
    uint16_t phiras;
    uint16_t thystame;
} inventory_attr_t;

/// @brief Enumeration representing the inventory element's indexes.
///
/// @var food The food counter.
/// @var linemate The Linemate counter.
/// @var deraumere The Deraumere counter.
/// @var sibur The Sibur counter.
/// @var mendiane The Mendiane counter.
/// @var phiras The Phiras counter.
/// @var thystame The Thystame counter.
typedef enum inventory_index_s {
    FOOD_IDX,
    LINEMATE_IDX,
    DERAUMERE_IDX,
    SIBUR_IDX,
    MENDIANE_IDX,
    PHIRAS_IDX,
    THYSTAME_IDX,
    INVENTORY_SIZE
} PACKED inventory_index_t;

/// @brief Union representing the inventory (accessible by attribute or array).
///
/// @var attr In order to access the inventory by attribute.
/// @var arr In order to access the inventory by array (in order to iterate).
typedef union inventory_s {
    inventory_attr_t attr;
    uint16_t arr[INVENTORY_SIZE];
} inventory_t;
