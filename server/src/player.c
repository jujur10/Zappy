/*
** EPITECH PROJECT, 2024
** player.c
** File description:
** player.c.
*/
#include "server.h"

int32_t get_player_by_socket(const server_t PTR server, uint16_t sock)
{
    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (sock == server->players[i].sock)
            return (int32_t)i;
    }
    return -1;
}

void add_time_limit_to_player(double server_time_units,
    uint32_t time_limit, player_t *player)
{
    player->blocking_time = server_time_units + time_limit;
}
