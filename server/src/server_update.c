/*
** EPITECH PROJECT, 2024
** server_update.c
** File description:
** server_update.c.
*/
#include <string.h>

#include "server.h"
#include "clock.h"
#include "logging.h"
#include "game_settings.h"

/// @brief Function which update the in game time. (based on real time clock
/// and frequency parameter).
///
/// @param server The server to update.
static void update_game_clock(server_t PTR server)
{
    static bool first_time = true;
    static struct timespec old_clock = {};

    clock_gettime(CLOCK_MONOTONIC, &server->clock);
    old_clock = (true == first_time) ? server->clock : old_clock;
    server->time_units += real_time_clock_to_time_units(server->frequency,
    &server->clock, &old_clock);
    old_clock = server->clock;
    first_time = false;
    LOGF("Accumulated time units : %lf", server->time_units)
}

/// @brief Function returning if the team in parameter has win.
///
/// @param server The server structure.
/// @param team The team.
/// @return True if win, false if not.
static bool did_the_team_win(server_t PTR server, const team_t PTR team)
{
    uint16_t count = 0;

    for (uint16_t player_idx = 0; player_idx < team->nb_of_players;
    player_idx++) {
        if (MAX_AI_LVL == server->players[team->players_idx[player_idx]].level)
            count++;
    }
    return count >= 6;
}

/// @brief Function used to get the game status (if a team have won or not).
///
/// @param server The server structure.
/// @return -1 If no team has won, else team's index.
static int32_t get_game_status(server_t PTR server)
{
    const team_t *team;

    for (uint16_t team_idx = 0; team_idx < server->args->nb_of_teams;
    team_idx++) {
        team = &server->teams[team_idx];
        if (true == did_the_team_win(server, team))
            return team_idx;
    }
    return -1;
}

/// @brief Function which sends to GUIs the events of seg.
///
/// @param server The server structure.
/// @param winning_team The winning team.
static void send_seg_to_guis(server_t PTR server, uint32_t winning_team)
{
    string_t string;
    msg_t message;

    init_string_from_chars(&string, "seg ", 4);
    append_to_string_from_chars(&string, server->teams[winning_team].name,
        (uint32_t)strlen(server->teams[winning_team].name));
    append_to_string_from_chars(&string, "\n", 1);
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        add_msg_to_queue(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}

void update_server(server_t PTR server)
{
    int32_t team_idx = get_game_status(server);

    if (-1 != team_idx)
        send_seg_to_guis(server, team_idx);
    update_game_clock(server);
    update_map(server->time_units, &server->map, &server->generated_buffers);
}
