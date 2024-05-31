/*
** EPITECH PROJECT, 2024
** team.h
** File description:
** team.h.
*/

#pragma once

#include <stdint.h>

#include "arguments.h"
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
    uint16_t ARRAY players_idx;
    uint16_t nb_of_players;
    uint16_t max_nb_of_players;
    uint32_t nb_of_eggs;
    coordinates_t ARRAY eggs_coordinates;
} team_t;

/// @brief Function which initializes teams.
/// @param args The parsed program parameters.
/// @param teams The teams to initialize.
/// @return 0 on success, 1 on failure.
uint8_t init_teams(const argument_t *args, team_t **teams);

/// @brief Function which destroys teams.
/// @param args The parsed program parameters.
/// @param teams The teams to destroys.
void destroy_teams(const argument_t PTR args, team_t ARRAY teams);

/// @brief Function which returns the team index of the name's team.
/// @param teams The array of teams.
/// @param nb_of_teams The array's lenght.
/// @param searched_name The team name we are searching.
/// @param len The searched name length.
int32_t get_team_index_by_name(const team_t ARRAY teams, uint32_t nb_of_teams,
    const char ARRAY searched_name, uint32_t len);
