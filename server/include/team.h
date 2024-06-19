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
#include "style/status.h"

/// @brief Redefinition.
typedef struct server_s server_t;

/// @brief Structure representing an egg.
///
/// @var index The index of the egg.
/// @var egg_coordinates The egg coordinates.
typedef struct egg_s {
    uint16_t index;
    coordinates_t egg_coordinates;
} egg_t;

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
    uint16_t nb_of_eggs;
    uint16_t nb_of_allocated_eggs;
    egg_t ARRAY eggs;
    uint16_t egg_counter;
} team_t;

/// @brief Function which initializes teams.
/// @param args The parsed program parameters.
/// @param teams The teams to initialize.
/// @return 0 on success, 1 on failure.
uint8_t init_teams(const argument_t PTR args, team_t PTR ARRAY teams);

/// @brief Function which destroys teams.
/// @param args The parsed program parameters.
/// @param teams The teams to destroys.
void destroy_teams(const argument_t PTR args, team_t ARRAY teams);

/// @brief Function that returns the index of the searched team.
/// @param teams The array of teams.
/// @param nb_of_teams The array's length.
/// @param searched_name The team name we are searching.
/// @param len The searched name length.
int32_t get_team_index_by_name(const team_t ARRAY teams, uint32_t nb_of_teams,
    const char ARRAY searched_name, uint32_t len);

/// @brief Function that returns the number of unused slot for the team.
/// @param team The team.
/// @return The number of unused slots for the team.
uint16_t get_nb_of_unused_slot(const team_t PTR team);

/// @brief Function which adds a player to a team (if possible).
///
/// @param server The server structure.
/// @param team_idx The team we want to assign the player to.
/// @param player_idx The player to add to the team.
/// @return SUCCESS if the player has been added successfully, FAILURE if not.
status_t add_player_to_team(server_t PTR server, uint16_t team_idx,
    uint16_t player_idx);

/// @brief Function used to remove a player from a team.
///
/// @param server The server structure.
/// @param player_idx The player to add to the team.
void remove_player_from_team(server_t PTR server, uint16_t player_idx);

/// @brief Function used to destroy an egg properly with a notification to
/// the guis.
///
/// @param server The server structure.
/// @param team The concerned team.
/// @param egg_index The egg index in the team structure to destroy.
void destroy_egg(server_t PTR server, team_t PTR team, uint16_t egg_index);
