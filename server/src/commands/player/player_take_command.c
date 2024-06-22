/*
** EPITECH PROJECT, 2024
** player_take_command.c
** File description:
** player_take_command.c.
*/
#include <stdlib.h>
#include "server.h"

#include "game_settings.h"
#include "utils/itoa/fast_itoa.h"

void execute_player_take_command(server_t PTR server, uint16_t player_idx,
    const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    resources_index_t resource_idx = get_resource_index_from_str
        (command->argument.ptr);
    resources_t *tile = get_resource_tile_by_coordinates
        (&server->map, &player->coordinates);

    free(command->argument.ptr);
    if (R_STRUCT_SIZE == resource_idx)
        return player_ko_response(server, player);
    if (tile->arr[resource_idx] > 0) {
        player_take_set_to_tile(player, tile, resource_idx,
            PLAYER_TAKE_OBJ_CMD);
        send_pgt_to_guis(server, player, resource_idx);
        send_pin_to_guis(server, player);
        send_bct_to_guis(server, &player->coordinates);
        add_time_limit_to_player(server->time_units, PLAYER_TAKE_WAIT, player);
        return player_ok_response(server, player);
    }
    player_ko_response(server, player);
}
