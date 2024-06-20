/*
** EPITECH PROJECT, 2024
** player_fork_command.c
** File description:
** player_fork_command.c.
*/
#include <stdlib.h>

#include "server.h"
#include "game_settings.h"
#include "commands/command_utils.h"

/// @brief Function which sends to GUIs the events of enw.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
static void send_enw_to_guis(server_t PTR server,
    const egg_t PTR egg, const player_t PTR player)
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

/// @brief Function which add an egg to a team.
///
/// @param server The server structure.
/// @param team The team where you have to add the egg.
/// @param egg_coordinates The egg coordinates.
/// @param player The player.
static void add_egg_to_team(server_t PTR server, team_t PTR team,
    const coordinates_t PTR egg_coordinates, player_t PTR player)
{
    egg_t *new_egg_array;

    if (team->nb_of_eggs == team->nb_of_allocated_eggs) {
        new_egg_array = realloc(team->eggs, sizeof(egg_t)
        * (team->nb_of_allocated_eggs + 5));
        if (NULL == new_egg_array)
            return;
        team->nb_of_allocated_eggs += 5;
        team->eggs = new_egg_array;
    }
    team->eggs[team->nb_of_eggs].egg_coordinates = *egg_coordinates;
    team->eggs[team->nb_of_eggs].index = server->egg_counter;
    send_enw_to_guis(server, &team->eggs[team->nb_of_eggs], player);
    server->egg_counter++;
    team->nb_of_eggs++;
}

/// @brief Function which sends to GUIs the events of pfk.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
static void send_pfk_to_guis(server_t PTR server, const player_t PTR player)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pfk ";
    uint32_t count = 4;

    write_nb_to_buffer(player->sock, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

void execute_player_fork_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[player->team_idx];

    send_pfk_to_guis(server, player);
    add_egg_to_team(server, team, &player->coordinates, player);
    add_buffer_to_queue(&player->queue, &server->generated_buffers
        .buffers[PRE_OK_RESPONSE]);
    add_time_limit_to_player(server->time_units, PLAYER_FORK_WAIT, player);
}
