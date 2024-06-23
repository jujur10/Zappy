/*
** EPITECH PROJECT, 2024
** gui_plv_command.c
** File description:
** gui_plv_command.c.
*/
#include "server.h"
#include "player.h"
#include "commands/gui_commands.h"
#include "msg.h"
#include "commands/command_utils.h"

status_t create_gui_plv_message(server_t PTR server,
    uint16_t player_sock, msg_t PTR message)
{
    char buffer[4 + (2 * UINT32_MAX_DIGITS) + 1] = "plv ";
    uint32_t count = 4;
    int32_t player_idx = get_player_by_socket(server, player_sock);
    const player_t *current_player;

    if (-1 == player_idx) {
        create_message("sbp\n", 4, message);
        return FAILURE;
    }
    current_player = &server->players[player_idx];
    write_nb_to_buffer(player_sock, buffer, &count);
    write_nb_to_buffer(current_player->level + 1, buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, message);
    return SUCCESS;
}

void execute_gui_plv_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    msg_t message = {};

    create_gui_plv_message(server, command->args[0], &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
