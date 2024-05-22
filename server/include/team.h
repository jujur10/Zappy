/*
** EPITECH PROJECT, 2024
** team.h
** File description:
** team.h.
*/

#pragma once

#include <stdint.h>

#include "coordinates.h"

/// @brief Structure representing teams.
///
/// @var name The team's name.
/// @var players_idx The player's indexes.
/// @var nb_of_players The current number of players.
/// @var max_nb_of_players The maximal number of players for the team.
/// @var eggs_coordinates The eggs coordinates.
/// @var nb_of_eggs The number of eggs.
typedef struct team_s {
    const char ARRAY name;
    uint32_t ARRAY players_idx;
    uint32_t nb_of_players;
    uint32_t max_nb_of_players;
    coordinates_t ARRAY eggs_coordinates;
    uint32_t nb_of_eggs;
} team_t;
