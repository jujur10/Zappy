/*
** EPITECH PROJECT, 2024
** server.h
** File description:
** server.h.
*/

#pragma once

#include <stdint.h>
#include <sys/select.h>

#include "utils/pre_generate/pre_generate.h"
#include "arguments.h"
#include "gui.h"
#include "map.h"
#include "new_clients.h"
#include "player.h"
#include "team.h"
#include "style/macros.h"

// Authentication timeout in seconds and nanoseconds.
#define AUTH_TIMEOUT_SEC 0
#define AUTH_TIMEOUT_NS 200000000

// Delay to force to close the server after the end of game event.
#define END_GAME_TIMEOUT_SEC 0
#define END_GAME_TIMEOUT_NS 500000000

// GUI team name.
#define GUI_TEAM "GRAPHIC\n"

// Set the max client macro based on FD_SETSIZE.
#define MAX_CLIENTS FD_SETSIZE

// Set the max number of new clients.
#define MAX_NEW_CLIENTS 200

/// @brief Structure representing server.
///
/// @var sock The server's socket.
/// @var clock The server's clock.
/// @var current_socks The server's file descriptor set.
/// @var max_client The maximal fd to watch.
/// @var time_units The current time units since start.
/// @var frequency The actual frequency.
/// @var generated_buffers The pre-generated buffers.
/// @var args The pointer on program arguments structure.
/// @var clients The new clients.
/// @var nb_clients The number of clients inside the array.
/// @var players The players.
/// @var nb_players The number of players inside the array.
/// @var guis The GUIs.
/// @var nb_guis The number of GUIs inside the array.
/// @var teams The teams.
/// @var map The map.
typedef struct server_s {
    int sock;
    struct timespec clock;
    fd_set current_socks;
    int max_client;
    double time_units;
    uint32_t frequency;
    generated_buffers_t generated_buffers;
    const argument_t PTR args;
    new_client_t clients[MAX_CLIENTS];
    uint16_t nb_clients;
    player_t players[MAX_CLIENTS];
    uint16_t nb_players;
    gui_t guis[MAX_CLIENTS];
    uint16_t nb_guis;
    team_t ARRAY teams;
    uint16_t egg_counter;
    map_t map;
} server_t;

/// @brief Function which initialize preliminaries, run the main loop and exit.
/// @param args The parsed program parameters.
/// @return 0 on success, 84 on failure.
uint8_t run_server(const argument_t PTR args);

/// @brief Function which is used to handle new connections.
///
/// @param server The server structure.
void on_connection(server_t PTR server);

/// @brief Function used to send a server shutdown request.
void send_quit_server_request(void);
