/*
** EPITECH PROJECT, 2024
** player_fork_command.c
** File description:
** player_fork_command.c.
*/
#include "server.h"
#include "game_settings.h"
#include "commands/command_utils.h"

void execute_player_fork_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[player->team_idx];

    send_pfk_to_guis(server, player);
    if (SUCCESS == try_add_egg_to_team(server, team, &player->coordinates))
        send_enw_to_guis(server, &team->eggs[team->nb_of_eggs - 1], player);
    player_ok_response(server, player);
    add_time_limit_to_player(server->time_units, PLAYER_FORK_WAIT, player);
}
