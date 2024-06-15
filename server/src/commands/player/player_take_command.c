/*
** EPITECH PROJECT, 2024
** player_take_command.c
** File description:
** player_take_command.c.
*/
#include "server.h"

#include "queue/msg_queue.h"
#include "game_settings.h"

void execute_player_take_command(server_t PTR server, uint16_t player_idx,
    const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    resources_index_t resource_idx = get_resource_index_from_str
        (command->argument.ptr);
    resources_t *current_tile = get_resource_tile_by_coordinates
        (&server->map, &player->coordinates);

    if (R_STRUCT_SIZE == resource_idx)
        return (void)add_buffer_to_queue(&player->queue,
            &server->generated_buffers.buffers[PRE_KO_RESPONSE]);
    if (current_tile->arr[resource_idx] > 0) {
        current_tile->arr[resource_idx]--;
        player->inventory.arr[resource_idx]++;
        add_time_limit_to_player(server->time_units, PLAYER_TAKE_WAIT, player);
        return (void)add_buffer_to_queue(&player->queue,
            &server->generated_buffers.buffers[PRE_OK_RESPONSE]);
    }
    add_buffer_to_queue(&player->queue, &server->generated_buffers
        .buffers[PRE_KO_RESPONSE]);
    server->map.has_been_modified = true;
}
