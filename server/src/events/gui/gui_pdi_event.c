/*
** EPITECH PROJECT, 2024
** gui_pdi_event.c
** File description:
** gui_pdi_event.c.
*/
#include "server.h"
#include "commands/command_utils.h"

void send_pdi_to_guis(server_t PTR server, uint32_t player_idx)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pdi ";
    uint32_t count = 4;

    write_nb_to_buffer(server->players[player_idx].sock, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}
