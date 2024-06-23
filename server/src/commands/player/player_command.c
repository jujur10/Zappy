/*
** EPITECH PROJECT, 2024
** player_command.c
** File description:
** player_command.c.
*/
#include <string.h>

#include "style/status.h"
#include "server.h"
#include "commands/player_commands.h"

static void (* const player_commands[PLAYER_NB_OF_CMD])(server_t PTR server,
    uint16_t player_idx, const player_command_t PTR command) = {
    execute_player_none_command, execute_player_forward_command,
    execute_player_right_command, execute_player_left_command,
    execute_player_look_command, execute_player_inventory_command,
    execute_player_broadcast_command, execute_player_connect_nbr_command,
    execute_player_fork_command, execute_player_eject_command,
    execute_player_take_command, execute_player_set_command,
    execute_player_incantation_command, execute_player_frequency_command,
    execute_player_direction_command
};

status_t get_next_player_command(
    player_command_buffer_t PTR player_command_buffer,
    player_command_t PTR next_command)
{
    if (0 == player_command_buffer->nb_of_command)
        return FAILURE;
    *next_command = player_command_buffer->commands[0];
    memmove(player_command_buffer->commands,
        player_command_buffer->commands + 1,
        sizeof(player_command_t) *
        (player_command_buffer->nb_of_command - 1));
    player_command_buffer->nb_of_command--;
    memset(player_command_buffer->commands +
        player_command_buffer->nb_of_command, 0, sizeof(player_command_t));
    return SUCCESS;
}

void execute_player_command(server_t PTR server, uint16_t player_idx,
    const player_command_t PTR command)
{
    return player_commands[command->command](server, player_idx, command);
}
