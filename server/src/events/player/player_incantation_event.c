/*
** EPITECH PROJECT, 2024
** player_incantation_event.c
** File description:
** player_incantation_event.c.
*/
#include "commands/command_utils.h"
#include "server.h"
#include "game_settings.h"

/// @brief Function which sends to GUIs the events of pie.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
static void send_pie_to_guis(server_t PTR server,
    const coordinates_t PTR coordinates, status_t status)
{
    msg_t message;
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1] = "pie ";
    uint32_t count = 4;

    write_nb_to_buffer(coordinates->x, msg_content, &count);
    write_nb_to_buffer(coordinates->y, msg_content, &count);
    write_nb_to_buffer(status, msg_content, &count);
    msg_content[count - 1] = '\n';
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(msg_content, count, &message);
        add_msg_to_queue(&server->guis[i].queue, &message);
    }
}

/// @brief Function used to elevate players who made incantation.
///
/// @param players The array of players.
/// @param nb_of_players The len of the array of players.
/// @param player_idx The player index of the player who made the incantation.
static void elevate_players(player_t ARRAY players, uint16_t nb_of_players,
    uint16_t player_idx)
{
    msg_t message = {};
    char msg_content[sizeof("Current level: ") + UINT32_MAX_DIGITS] =
        "Current level: ";
    uint32_t count = 15;
    const player_t *player = &players[player_idx];

    write_nb_to_buffer(player->level + 1, msg_content, &count);
    msg_content[count - 1] = '\n';
    for (uint16_t i = 0; i < nb_of_players; i++) {
        if (i == player_idx)
            continue;
        if (false == is_coordinates_equal(&player->coordinates,
            &players[i].coordinates))
            continue;
        if (player->level != players[i].level)
            continue;
        players[i].level++;
        create_message(msg_content, count, &message);
        add_msg_to_queue(&players[i].queue, &message);
    }
}

void execute_player_incantation_event(server_t PTR server, uint32_t player_idx)
{
    player_t *player = &server->players[player_idx];

    if (true == verify_requirements(server, (uint16_t)player_idx)) {
        if (player->level < MAX_AI_LVL - 1)
            player->level++;
        elevate_players(server->players, server->nb_players,
            (uint16_t)player_idx);
        send_pie_to_guis(server, &player->coordinates, SUCCESS);
        return;
    }
    send_pie_to_guis(server, &player->coordinates, FAILURE);
    return player_ko_response(server, player);
}
