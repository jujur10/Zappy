/*
** EPITECH PROJECT, 2024
** server.h
** File description:
** server.h.
*/

#pragma once

#include <stdint.h>
#include <sys/select.h>

#include "player.h"
#include "new_clients.h"
#include "gui.h"
#include "team.h"
#include "map.h"
#include "arguments.h"

// Authentication timeout in seconds and nanoseconds.
#define AUTH_TIMEOUT_SEC 0
#define AUTH_TIMEOUT_NS 200000000

// GUI team name.
#define GUI_TEAM "GRAPHIC\n"

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

// Set the max client macro based on FD_SETSIZE.
#define MAX_CLIENTS FD_SETSIZE

/// @brief Structure representing server.
///
/// @var sock The server's socket.
/// @var clock The server's clock.
/// @var current_socks The server's file descriptor set.
/// @var max_client The maximal fd to watch.
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
    const argument_t PTR args;
    new_client_t clients[MAX_CLIENTS];
    uint16_t nb_clients;
    player_t players[MAX_CLIENTS];
    uint16_t nb_players;
    gui_t guis[MAX_CLIENTS];
    uint16_t nb_guis;
    team_t ARRAY teams;
    map_t map;
} server_t;

/// @brief Function which initialize preliminaries, run the main loop and exit.
/// @param args The parsed program parameters.
/// @return 0 on success, 84 on failure.
uint8_t run_server(const argument_t PTR args);
