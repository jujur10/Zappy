/*
** EPITECH PROJECT, 2024
** gui_pdr_event.c
** File description:
** gui_pdr_event.c.
*/
#include "server.h"
#include "commands/command_utils.h"

void send_pdr_to_guis(server_t PTR server, const player_t PTR player,
    resources_index_t resource_index)
{
    char msg_content[4 + (2 * UINT32_MAX_DIGITS) + 1] = "pdr ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(resource_index, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}
