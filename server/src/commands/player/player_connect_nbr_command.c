/*
** EPITECH PROJECT, 2024
** player_connect_nbr_command.c
** File description:
** player_connect_nbr_command.c.
*/
#include <stdint.h>

#include "server.h"
#include "commands/player_commands.h"
#include "utils/itoa/fast_itoa.h"

void execute_player_connect_nbr_command(server_t PTR server,
    uint16_t player_idx, UNUSED const player_command_t PTR command)
{
    char buffer[11];
    player_t *player = &server->players[player_idx];
    const team_t *team = &server->teams[player->team_idx];
    uint16_t nb_of_remaining_slot = get_nb_of_unused_slot(team);
    uint64_t wrote;
    msg_t message = {};

    wrote = fast_itoa_u32(nb_of_remaining_slot, buffer);
    buffer[wrote] = '\n';
    create_message(buffer, (uint32_t)wrote, &message);
    add_msg_to_queue(&player->queue, &message);
}
