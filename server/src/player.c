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
    uint32_t time_limit, player_t PTR player)
{
    player->blocking_time = server_time_units + time_limit;
}

void get_next_player_coordinates(const map_t PTR map,
    const player_t PTR player, coordinates_t PTR new_coordinates)
{
    new_coordinates->x = (player->coordinates.x
        + (player->orientation == LOOK_RIGHT)
        - (player->orientation == LOOK_LEFT)
        + map->width) % map->width;
    new_coordinates->y = (player->coordinates.y
        + (player->orientation == LOOK_BOTTOM)
        - (player->orientation == LOOK_TOP)
        + map->height) % map->height;
}

void teleport_player(map_t PTR map, player_t PTR player,
    const coordinates_t PTR new_coordinates)
{
    const coordinates_t *player_coordinates = &player->coordinates;
    resources_t *current_tile = &map->tiles[(player_coordinates->y *
        map->width) + player_coordinates->x];
    resources_t *target_tile = &map->tiles[(new_coordinates->y *
        map->width) + new_coordinates->x];

    current_tile->attr.players -= (current_tile->attr.players > 0) ? 1 : 0;
    target_tile->attr.players++;
    player->coordinates = *new_coordinates;
    map->has_been_modified = true;
}

void player_take_set_to_tile(player_t PTR player, resources_t PTR tile,
    resources_index_t resource_index, player_command_base_t action)
{
    if (PLAYER_SET_OBJ_CMD == action) {
        tile->arr[resource_index]++;
        player->inventory.arr[resource_index]--;
    } else if (PLAYER_TAKE_OBJ_CMD == action) {
        tile->arr[resource_index]--;
        player->inventory.arr[resource_index]++;
    }
}
