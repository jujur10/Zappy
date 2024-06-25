/*
** EPITECH PROJECT, 2024
** end_of_the_game.c
** File description:
** end_of_the_game.c.
*/
#include <stdint.h>
#include <unistd.h>

#include "server.h"
#include "game_settings.h"
#include "events/gui_events.h"
#include "new_clients_handling.h"
#include "clock.h"
#include "signal_handler.h"

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
        if (MAX_AI_LVL == server->players[team->players_idx[player_idx]]
        .level - 1)
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

void check_end_of_the_game(server_t PTR server)
{
    int32_t team_idx = get_game_status(server);
    static struct timespec current_clock = {};
    static bool game_ended = false;

    if (true == game_ended) {
        if (-1 != pipe_signals[1] && true == is_timeout_exceed(&server->clock,
        &current_clock))
            send_quit_server_request() SEMICOLON return;
    }
    if (-1 == team_idx)
        return;
    FD_CLR(server->sock, &server->current_socks);
    close(server->sock);
    for (uint32_t i = 0; i < server->nb_clients; i++)
        destroy_new_client(server, i, false);
    send_seg_to_guis(server, team_idx);
    send_end_of_game_to_players(server);
    game_ended = true;
    current_clock = server->clock;
    add_to_clock(&current_clock, END_GAME_TIMEOUT_SEC, END_GAME_TIMEOUT_NS);
}
