/*
** EPITECH PROJECT, 2024
** gui_enw_event.c
** File description:
** gui_enw_event.c.
*/
#include "server.h"
#include "commands/command_utils.h"

void send_enw_to_guis(server_t PTR server, const egg_t PTR egg,
    const player_t PTR player)
{
    char msg_content[4 + (4 * UINT32_MAX_DIGITS) + 1] = "enw ";
    uint32_t count = 4;

    write_nb_to_buffer(egg->index, msg_content, &count);
    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(egg->egg_coordinates.x, msg_content, &count);
    write_nb_to_buffer(egg->egg_coordinates.y, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}
