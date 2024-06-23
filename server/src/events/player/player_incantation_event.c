/*
** EPITECH PROJECT, 2024
** player_incantation_event.c
** File description:
** player_incantation_event.c.
*/
#include "commands/command_utils.h"
#include "server.h"
#include "game_settings.h"
#include "commands/player_commands.h"

/// @brief Function which sends to GUIs the events of pie.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
/// @param status True for success or false for failure.
static void send_pie_to_guis(server_t PTR server,
    const coordinates_t PTR coordinates, bool status)
{
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1] = "pie ";
    uint32_t count = 4;

    write_nb_to_buffer(coordinates->x, msg_content, &count);
    write_nb_to_buffer(coordinates->y, msg_content, &count);
    write_nb_to_buffer(status, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

/// @brief Function which sends to GUIs the events of plv.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
static void send_plv_to_guis(server_t PTR server, const player_t PTR player)
{
    char msg_content[4 + (2 * UINT32_MAX_DIGITS) + 1] = "plv ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    write_nb_to_buffer(player->level + 1, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

/// @brief Function used to elevate a player.
///
/// @param server The server structure.
/// @param player The player to elevate.
/// @param msg_content The message content to send.
/// @param len The message content length.
static void elevate_player(server_t PTR server, player_t PTR player,
    char ARRAY msg_content, uint32_t len)
{
    msg_t message = {};

    player->status = PLAYING;
    player->level++;
    send_plv_to_guis(server, player);
    create_message(msg_content, len, &message);
    add_msg_to_queue(&player->queue, &message);
}

/// @brief Function used to elevate players who made incantation.
///
/// @param players The array of players.
/// @param nb_of_players The len of the array of players.
/// @param player_idx The player index of the player who made the incantation.
static void elevate_players(server_t PTR server, player_t ARRAY players,
    uint16_t nb_of_players, uint16_t player_idx)
{
    char msg_content[sizeof("Current level: ") + UINT32_MAX_DIGITS] =
        "Current level: ";
    uint32_t count = 15;
    player_t *player = &players[player_idx];

    write_nb_to_buffer(player->level + 2, msg_content, &count);
    msg_content[count - 1] = '\n';
    for (uint16_t i = 0; i < nb_of_players; i++) {
        if (i == player_idx || false == is_coordinates_equal
        (&player->coordinates, &players[i].coordinates) || player->level !=
        players[i].level)
            continue;
        elevate_player(server, &players[i], msg_content, count);
    }
    elevate_player(server, player, msg_content, count);
}

/// @brief Function which remove the items needed for incantation.
///
/// @param tile The tile where the incantation is produced.
/// @param player_level The level (in order to remove the right number of
/// resources).
static void remove_items_from_tile(resources_t PTR tile, uint16_t player_level)
{
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++)
        tile->arr[i] -= elevation_requirements[player_level].arr[i];
}

void execute_player_incantation_event(server_t PTR server, uint32_t player_idx)
{
    player_t *player = &server->players[player_idx];

    if (true == verify_requirements(server, (uint16_t)player_idx)) {
        remove_items_from_tile(get_resource_tile_by_coordinates
        (&server->map, &player->coordinates), player->level);
        if (player->level < MAX_AI_LVL - 1) {
            send_pie_to_guis(server, &player->coordinates, true);
            elevate_players(server, server->players, server->nb_players,
                (uint16_t)player_idx);
            return;
        }
    }
    send_pie_to_guis(server, &player->coordinates, false);
    return player_ko_response(server, player);
}
