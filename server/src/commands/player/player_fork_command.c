/*
** EPITECH PROJECT, 2024
** player_fork_command.c
** File description:
** player_fork_command.c.
*/
#include <stdlib.h>

#include "server.h"
#include "game_settings.h"

/// @brief Function which add an egg to a team.
///
/// @param team The team where you have to add the egg.
/// @param egg_coordinates The egg coordinates.
static void add_egg_to_team(team_t PTR team,
    const coordinates_t PTR egg_coordinates)
{
    coordinates_t *new_coordinate_array;

    if (team->nb_of_eggs == team->nb_of_allocated_eggs) {
        new_coordinate_array = realloc(team->eggs_coordinates, sizeof
        (coordinates_t) * (team->nb_of_allocated_eggs + 5));
        if (NULL == new_coordinate_array)
            return;
        team->nb_of_allocated_eggs += 5;
        team->eggs_coordinates = new_coordinate_array;
    }
    team->eggs_coordinates[team->nb_of_eggs] = *egg_coordinates;
    team->nb_of_eggs++;
}

void execute_player_fork_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[player->team_idx];

    add_egg_to_team(team, &player->coordinates);
    add_buffer_to_queue(&player->queue, &server->generated_buffers
        .buffers[PRE_OK_RESPONSE]);
    add_time_limit_to_player(server->time_units, PLAYER_FORK_WAIT, player);
}
