/*
** EPITECH PROJECT, 2024
** team_management.c
** File description:
** team_management.c.
*/
#include <string.h>

#include "server.h"

status_t add_player_to_team(server_t PTR server, uint16_t team_idx,
    uint16_t player_idx)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[team_idx];

    if (team->nb_of_players == team->max_nb_of_players ||
    0 == team->nb_of_eggs)
        return FAILURE;
    team->nb_of_eggs--;
    player->coordinates = team->eggs_coordinates[team->nb_of_eggs];
    team->players_idx[team->nb_of_players] = player_idx;
    team->nb_of_players++;
    server->players[player_idx].team_idx = team_idx;
    return SUCCESS;
}

void remove_player_from_team(server_t PTR server, uint16_t player_idx)
{
    team_t *team = &server->teams[server->players[player_idx].team_idx];

    for (uint16_t i = 0; i < team->nb_of_players; i++) {
        if (team->players_idx[i] == player_idx) {
            team->nb_of_players--;
            memmove(&team->players_idx[i],
                &team->players_idx[team->nb_of_players], sizeof(uint16_t));
            memset(&team->players_idx[team->nb_of_players], 0,
                sizeof(uint16_t));
        }
    }
}
