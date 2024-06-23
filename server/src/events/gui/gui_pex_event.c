/*
** EPITECH PROJECT, 2024
** gui_pex_event.c
** File description:
** gui_pex_event.c.
*/
#include "commands/command_utils.h"
#include "server.h"

void send_pex_to_guis(server_t PTR server, const player_t PTR player)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pex ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}
