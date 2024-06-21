/*
** EPITECH PROJECT, 2024
** player_forward_command.c
** File description:
** player_forward_command.c.
*/
#include "server.h"

#include "game_settings.h"
#include "commands/command_utils.h"
#include "events/gui_events.h"

void execute_player_forward_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    map_t *map = &server->map;
    coordinates_t new_coordinates;

    get_next_player_coordinates(map, player, &new_coordinates);
    teleport_player(map, player, &new_coordinates);
    player_ok_response(server, player);
    send_ppo_to_guis(server, player);
    add_time_limit_to_player(server->time_units, PLAYER_FORWARD_WAIT, player);
}
