/*
** EPITECH PROJECT, 2024
** player_connect_nbr_command.c
** File description:
** player_connect_nbr_command.c.
*/
#include <stdint.h>

#include "server.h"
#include "commands/player_commands.h"
#include "commands/command_utils.h"

void execute_player_connect_nbr_command(server_t PTR server,
    uint16_t player_idx, UNUSED const player_command_t PTR command)
{
    char buffer[11];
    player_t *player = &server->players[player_idx];
    const team_t *team = &server->teams[player->team_idx];
    uint16_t nb_of_remaining_slot = get_nb_of_unused_slot(team);
    uint32_t wrote = 0;
    msg_t message = {};

    write_nb_to_buffer(nb_of_remaining_slot, buffer, &wrote);
    buffer[wrote - 1] = '\n';
    create_message(buffer, wrote, &message);
    add_msg_to_queue(&player->queue, &message);
}
