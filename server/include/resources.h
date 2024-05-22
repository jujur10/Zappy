/*
** EPITECH PROJECT, 2024
** resources.h
** File description:
** resources.h.
*/

#pragma once

#include <stdint.h>

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
typedef struct resources_s {
    uint32_t foods;
    uint32_t linemates;
    uint32_t deraumeres;
    uint32_t siburs;
    uint32_t mendianes;
    uint32_t phiras;
    uint32_t thystames;
    uint32_t players;
} resources_t;
