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

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

// Set the max client macro based on FD_SETSIZE.
#define MAX_CLIENTS FD_SETSIZE

/// @brief Structure representing server.
///
/// @var sock The server's socket.
/// @var current_socks The server's file descriptor set.
/// @var max_client The maximal fd to watch.
/// @var clients The new clients.
/// @var players The players.
/// @var guis The GUIs.
/// @var teams The teams.
/// @var map The map.
typedef struct server_s {
    int sock;
    fd_set current_socks;
    uint32_t max_client;
    new_client_t clients[MAX_CLIENTS];
    player_t players[MAX_CLIENTS];
    gui_t guis[MAX_CLIENTS];
    team_t teams[MAX_CLIENTS];
    map_t map;
} server_t;

/// @brief Function which initialize preliminaries, run the main loop and exit.
/// @param args The parsed program parameters.
/// @return 0 on success, 1 on failure.
uint8_t run_server(const argument_t *args);
