/*
** EPITECH PROJECT, 2024
** resources.h
** File description:
** resources.h.
*/

#pragma once

#include <stdint.h>

#include "style/macros.h"

/// @brief Structure representing map resources.
///
/// @var foods The food counter.
/// @var linemates The Linemate counter.
/// @var deraumeres The Deraumere counter.
/// @var siburs The Sibur counter.
/// @var mendianes The Mendiane counter.
/// @var phiras The Phiras counter.
/// @var thystames The Thystame counter.
/// @var players The player counter.
typedef struct {
    uint32_t foods;
    uint32_t linemates;
    uint32_t deraumeres;
    uint32_t siburs;
    uint32_t mendianes;
    uint32_t phiras;
    uint32_t thystames;
    uint32_t players;
} resources_attr_t;

/// @brief Structure representing total map resources 64bits.
///
/// @var foods The food counter.
/// @var linemates The Linemate counter.
/// @var deraumeres The Deraumere counter.
/// @var siburs The Sibur counter.
/// @var mendianes The Mendiane counter.
/// @var phiras The Phiras counter.
/// @var thystames The Thystame counter.
/// @var players The player counter.
typedef struct {
    uint64_t foods;
    uint64_t linemates;
    uint64_t deraumeres;
    uint64_t siburs;
    uint64_t mendianes;
    uint64_t phiras;
    uint64_t thystames;
    uint64_t players;
} resources_attr64_t;

/// @brief Enumeration representing the resource structure indexes.
///
/// @var R_FOOD_IDX The food counter.
/// @var R_LINEMATE_IDX The Linemate counter.
/// @var R_DERAUMERE_IDX The Deraumere counter.
/// @var R_SIBUR_IDX The Sibur counter.
/// @var R_MENDIANE_IDX The Mendiane counter.
/// @var R_PHIRAS_IDX The Phiras counter.
/// @var R_THYSTAME_IDX The Thystame counter.
/// @var R_PLAYER_IDX The Player counter.
/// @var R_INVENTORY_SIZE The number of attributes.
typedef enum resources_index_s {
    R_FOOD_IDX,
    R_LINEMATE_IDX,
    R_DERAUMERE_IDX,
    R_SIBUR_IDX,
    R_MENDIANE_IDX,
    R_PHIRAS_IDX,
    R_THYSTAME_IDX,
    R_PLAYER_IDX,
    R_STRUCT_SIZE
} PACKED resources_index_t;

/// @brief Macros to represent the resources as array of chars.
#define FOOD_STR "food"
#define LINEMATE_STR "linemate"
#define DERAUMERE_STR "deraumere"
#define SIBUR_STR "sibur"
#define MENDIANE_STR "mendiane"
#define PHIRAS_STR "phiras"
#define THYSTAME_STR "thystame"

/// @brief Union representing the resources (accessible by attribute or array).
///
/// @var attr In order to access the resources by attribute.
/// @var arr In order to access the resources by array (in order to iterate).
typedef union {
    resources_attr_t attr;
    uint32_t arr[R_STRUCT_SIZE];
} resources_t;

/// @brief Union representing the total resources (accessible by attribute or
/// array).
///
/// @var attr In order to access the resources by attribute.
/// @var arr In order to access the resources by array (in order to iterate).
typedef union {
    resources_attr64_t attr;
    uint64_t arr[R_STRUCT_SIZE];
} resources64_t;

/// @brief Enumeration representing map resources proportions.\n
/// Percentile multiplied by RES_PROPORTION_MULTIPLIER.
///
/// @var NB_OF_FOODS The Food proportion.
/// @var NB_OF_LINEMATES The Linemate proportion.
/// @var NB_OF_DERAUMERES The Deraumere proportion.
/// @var NB_OF_SIBURS The Sibur proportion.
/// @var NB_OF_MENDIANES The Mendiane proportion.
/// @var NB_OF_PHIRAS The Phiras proportion.
/// @var NB_OF_THYSTAMES The Thystame proportion.
typedef enum {
    NB_OF_FOODS = 50,
    NB_OF_LINEMATES = 30,
    NB_OF_DERAUMERES = 15,
    NB_OF_SIBURS = 10,
    NB_OF_MENDIANES = 10,
    NB_OF_PHIRAS = 8,
    NB_OF_THYSTAMES = 5,
} resources_proportions_t;

/// @brief The resource multiplier of resources_proportions_t enumeration.
#define RES_PROPORTION_MULTIPLIER 100

/// @brief Macro to calculate the resource ratio.
#define GET_RES_PROPORTION(x) (((double)(x)) / RES_PROPORTION_MULTIPLIER)

/// @brief Function which returns the resource index resulting of the string
/// comparison.
///
/// @param string The string to convert.
/// @return The resource index representing the string or R_STRUCT_SIZE on
/// failure.
resources_index_t get_resource_index_from_str(const char *string);

/// @brief Calculate the number of required tiles for a depth.
/// Sequence un+1 : n (n + 1) = n + 2
/// Sequence un : n (n) = (n * 2) + 1
///
/// @param depth The depth.
/// @return The number of required tiles for the depth.
uint32_t get_nb_of_tiles_for_a_depth(uint32_t depth);

/// @brief Calculate the total number of tiles required for a level.
/// ((U0 + Un) * (n + 1)) / 2
///
/// @param level The actual level.
/// @return The total number of tiles.
uint32_t get_total_nb_of_tiles_required(uint8_t level);
