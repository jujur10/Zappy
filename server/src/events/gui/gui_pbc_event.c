/*
** EPITECH PROJECT, 2024
** gui_pbc_event.c
** File description:
** gui_pbc_event.c.
*/
#include "server.h"
#include "commands/command_utils.h"

void send_pbc_to_guis(server_t PTR server,
    const buffer_t PTR message_buffer, uint32_t player_idx)
{
    msg_t message = {};
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pbc ";
    uint32_t count = 4;
    const player_t *player = &server->players[player_idx];
    string_t string;

    write_nb_to_buffer(player->sock, msg_content, &count);
    init_string_from_chars(&string, msg_content, count);
    append_to_string_from_chars(&string, message_buffer->ptr + sizeof
    (MESSAGE_STR) - 1, message_buffer->len - sizeof(MESSAGE_STR) + 1);
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        queue_push(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}
