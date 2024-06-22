/*
** EPITECH PROJECT, 2024
** player_fork_command.c
** File description:
** player_fork_command.c.
*/
#include <stdlib.h>

#include "server.h"
#include "game_settings.h"
#include "commands/command_utils.h"

/// @brief Function which add an egg to a team.
///
/// @param server The server structure.
/// @param team The team where you have to add the egg.
/// @param egg_coordinates The egg coordinates.
/// @param player The player.
static void add_egg_to_team(server_t PTR server, team_t PTR team,
    const coordinates_t PTR egg_coordinates, player_t PTR player)
{
    egg_t *new_egg_array;

    if (team->nb_of_eggs == team->nb_of_allocated_eggs) {
        new_egg_array = realloc(team->eggs, sizeof(egg_t)
        * (team->nb_of_allocated_eggs + 5));
        if (NULL == new_egg_array)
            return;
        team->nb_of_allocated_eggs += 5;
        team->eggs = new_egg_array;
    }
    team->eggs[team->nb_of_eggs].egg_coordinates = *egg_coordinates;
    team->eggs[team->nb_of_eggs].index = server->egg_counter;
    send_enw_to_guis(server, &team->eggs[team->nb_of_eggs], player);
    server->egg_counter++;
    team->nb_of_eggs++;
}

void execute_player_fork_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[player->team_idx];

    send_pfk_to_guis(server, player);
    add_egg_to_team(server, team, &player->coordinates, player);
    player_ok_response(server, player);
    add_time_limit_to_player(server->time_units, PLAYER_FORK_WAIT, player);
}
