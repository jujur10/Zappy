/*
** EPITECH PROJECT, 2024
** player_forward_command.c
** File description:
** player_forward_command.c.
*/
#include "server.h"

#include "queue/msg_queue.h"
#include "game_settings.h"
#include "commands/command_utils.h"

/// @brief Function which sends to GUIs the events of ppo.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
static void send_ppo_to_guis(server_t PTR server, const player_t PTR player)
{
    msg_t message = {};

    if (SUCCESS == create_gui_ppo_message(server, player->sock, &message))
        send_message_to_guis(server, message.ptr, message.len);
}

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
