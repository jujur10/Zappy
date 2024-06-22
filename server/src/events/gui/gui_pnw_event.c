/*
** EPITECH PROJECT, 2024
** gui_pnw_event.c
** File description:
** gui_pnw_event.c.
*/
#include <stdint.h>
#include <string.h>

#include "server.h"
#include "msg.h"
#include "commands/gui_commands.h"
#include "utils/string/string.h"
#include "commands/command_utils.h"

void create_gui_pnw_message(server_t PTR server, const player_t PTR player,
    msg_t PTR message)
{
    char msg_content[4 + (5 * UINT32_MAX_DIGITS) + 1] = "pnw ";
    uint32_t count = 4;
    string_t string;

    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(player->coordinates.x, msg_content, &count);
    write_nb_to_buffer(player->coordinates.y, msg_content, &count);
    write_nb_to_buffer(player->orientation + 1, msg_content, &count);
    write_nb_to_buffer(player->level + 1, msg_content, &count);
    init_string_from_chars(&string, msg_content, count);
    append_to_string_from_chars(&string, server->teams[player->team_idx].name,
        (uint32_t)strlen(server->teams[player->team_idx].name));
    append_to_string_from_chars(&string, "\n", 1);
    create_message(string.ptr, string.len, message);
}

void send_pnw_to_guis(server_t PTR server, const team_t PTR team,
    uint32_t player_idx)
{
    msg_t message = {};
    char msg_content[4 + (5 * UINT32_MAX_DIGITS) + 1] = "pnw ";
    uint32_t count = 4;
    const player_t *player = &server->players[player_idx];
    string_t string;

    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(player->coordinates.x, msg_content, &count);
    write_nb_to_buffer(player->coordinates.y, msg_content, &count);
    write_nb_to_buffer(player->orientation + 1, msg_content, &count);
    write_nb_to_buffer(player->level + 1, msg_content, &count);
    init_string_from_chars(&string, msg_content, count);
    append_to_string_from_chars(&string, team->name,
        (uint32_t)strlen(team->name));
    append_to_string_from_chars(&string, "\n", 1);
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        add_msg_to_queue(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}
