/*
** EPITECH PROJECT, 2024
** team_management.c
** File description:
** team_management.c.
*/
#include <string.h>

#include "server.h"
#include "commands/command_utils.h"

/// @brief Function which sends to GUIs the events of ebo.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
static void send_ebo_to_guis(server_t PTR server, uint16_t egg_index)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "ebo ";
    uint32_t count = 4;

    write_nb_to_buffer(egg_index, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

/// @brief Function which sends to GUIs the events of edi.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
static void send_edi_to_guis(server_t PTR server, uint16_t egg_index)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "edi ";
    uint32_t count = 4;

    write_nb_to_buffer(egg_index, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

/// @brief Function used to reallocate memory for team's player array..
///
/// @param team The team we want to increase capacity.
/// @return Success if reallocation is a success, Failure if not.
static status_t reallocate_team_players(team_t PTR team)
{
    uint16_t *new_player_index_array;

    new_player_index_array = realloc(team->players_idx, sizeof(uint16_t)
        * (team->nb_of_players + 1));
    if (NULL == new_player_index_array)
        return FAILURE;
    team->players_idx = new_player_index_array;
    return SUCCESS;
}

status_t try_add_player_to_team(server_t PTR server, uint16_t team_idx,
    uint16_t player_idx)
{
    player_t *player = &server->players[player_idx];
    team_t *team = &server->teams[team_idx];

    if (0 == team->nb_of_eggs)
        return FAILURE;
    team->nb_of_eggs--;
    player->coordinates = team->eggs[team->nb_of_eggs].egg_coordinates;
    send_ebo_to_guis(server, team->eggs[team->nb_of_eggs].index);
    memset(&team->eggs[team->nb_of_eggs], 0, sizeof(egg_t));
    reallocate_team_players(team);
    team->players_idx[team->nb_of_players] = player_idx;
    team->nb_of_players++;
    player->team_idx = team_idx;
    return SUCCESS;
}

void remove_player_from_team(server_t PTR server, uint16_t player_idx)
{
    team_t *team = &server->teams[server->players[player_idx].team_idx];

    for (uint16_t i = 0; i < team->nb_of_players; i++) {
        if (team->players_idx[i] == player_idx) {
            team->nb_of_players--;
            memmove(&team->players_idx[i],
                &team->players_idx[team->nb_of_players], sizeof(uint16_t));
            memset(&team->players_idx[team->nb_of_players], 0,
                sizeof(uint16_t));
        }
    }
}

void destroy_egg(server_t PTR server, team_t PTR team, uint16_t egg_index)
{
    send_edi_to_guis(server, egg_index);
    team->nb_of_eggs--;
    memmove(&team->eggs[egg_index], &team->eggs[team->nb_of_eggs],
        sizeof(egg_t));
    memset(&team->eggs[team->nb_of_eggs], 0, sizeof(egg_t));
}
