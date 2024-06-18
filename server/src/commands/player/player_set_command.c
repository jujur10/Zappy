/*
** EPITECH PROJECT, 2024
** player_set_command.c
** File description:
** player_set_command.c.
*/
#include "server.h"
#include "queue/msg_queue.h"
#include "game_settings.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

/// @brief Function which sends to GUIs the events of pdr.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
static void send_pdr_to_guis(server_t PTR server, const player_t PTR player,
    resources_index_t resource_index)
{
    msg_t message;
    char msg_content[4 + (2 * UINT32_MAX_DIGITS) + 1] = "pdr ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(resource_index, msg_content, &count);
    msg_content[count - 1] = '\n';
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(msg_content, count, &message);
        add_msg_to_queue(&server->guis[i].queue, &message);
    }
}

void execute_player_set_command(server_t PTR server, uint16_t player_idx,
    const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    resources_index_t resource_idx = get_resource_index_from_str
        (command->argument.ptr);
    resources_t *current_tile = get_resource_tile_by_coordinates
        (&server->map, &player->coordinates);
    const buffer_t *ko_buffer = &server->generated_buffers
        .buffers[PRE_KO_RESPONSE];

    if (R_STRUCT_SIZE == resource_idx)
        return (void)add_buffer_to_queue(&player->queue, ko_buffer);
    if (player->inventory.arr[resource_idx] > 0) {
        player->inventory.arr[resource_idx]--;
        current_tile->arr[resource_idx]++;
        send_pdr_to_guis(server, player, resource_idx);
        add_time_limit_to_player(server->time_units, PLAYER_SET_WAIT, player);
        return (void)add_buffer_to_queue(&player->queue,
            &server->generated_buffers.buffers[PRE_OK_RESPONSE]);
    }
    add_buffer_to_queue(&player->queue, ko_buffer);
    server->map.has_been_modified = true;
}
