/*
** EPITECH PROJECT, 2024
** gui_pic_event.c
** File description:
** gui_pic_event.c.
*/
#include "server.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

/// @brief Function used to write the player indexes (sockets) into the string.
///
/// @param server The server structure.
/// @param player_idx The player index.
/// @param string The string to write to.
static void write_ready_players_into_string(server_t PTR server,
    uint16_t player_idx, string_t PTR string)
{
    const player_t *player = &server->players[player_idx];
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1];
    uint32_t count;

    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (i != player_idx && true == is_coordinates_equal
        (&player->coordinates, &server->players[i].coordinates) &&
        player->level == server->players[i].level) {
            count = (uint32_t)fast_itoa_u32(server->players[i].sock,
                msg_content);
            msg_content[count] = ' ';
            append_to_string_from_chars(string, msg_content, count + 1);
        }
    }
    string->ptr[string->len - 1] = '\n';
}

/// @brief Function which sends to GUIs the events of pic.
///
/// @param server The server structure.
/// @param team The player's team.
/// @param player_idx The player index.
void send_pic_to_guis(server_t PTR server, uint32_t player_idx)
{
    msg_t message = {};
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1] = "pic ";
    uint32_t count = 4;
    const player_t *player = &server->players[player_idx];
    string_t string;

    write_nb_to_buffer(player->coordinates.x, msg_content, &count);
    write_nb_to_buffer(player->coordinates.y, msg_content, &count);
    write_nb_to_buffer(player->level + 1, msg_content, &count);
    init_string_from_chars(&string, msg_content, count);
    write_ready_players_into_string(server, (uint16_t)player_idx, &string);
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        queue_push(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}
