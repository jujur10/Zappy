/*
** EPITECH PROJECT, 2024
** gui_pfk_event.c
** File description:
** gui_pfk_event.c.
*/
#include "server.h"
#include "commands/command_utils.h"

void send_pfk_to_guis(server_t PTR server, const player_t PTR player)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pfk ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}
