/*
** EPITECH PROJECT, 2024
** player_forward_command.c
** File description:
** player_forward_command.c.
*/
#include "server.h"

#include "queue/msg_queue.h"
#include "game_settings.h"

void execute_player_forward_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    map_t *map = &server->map;
    coordinates_t new_coordinates;
    resources_t *tile = get_resource_tile_by_coordinates
        (map, &player->coordinates);
    resources_t *new_tile;

    get_next_player_coordinates(map, player, &new_coordinates);
    new_tile = get_resource_tile_by_coordinates(map, &new_coordinates);
    tile->attr.players--;
    new_tile->attr.players++;
    player->coordinates = new_coordinates;
    add_buffer_to_queue(&player->queue, &server->generated_buffers
        .buffers[PRE_OK_RESPONSE]);
    add_time_limit_to_player(server->time_units, PLAYER_FORWARD_WAIT, player);
    map->has_been_modified = true;
}
