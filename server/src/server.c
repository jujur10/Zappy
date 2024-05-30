/*
** EPITECH PROJECT, 2024
** server.c
** File description:
** server.c.
*/
#include <stdint.h>
#include <malloc.h>

#include "socket/sockets.h"
#include "server.h"
#include "arguments.h"
#include "logging.h"
#include "team.h"
#include "map.h"

/// @brief Function which initializes server.
/// @param args The parsed program parameters.
/// @param server The server structure to initialize.
/// @return 0 on success, 1 on failure.
static uint8_t init_server(const argument_t *args, server_t *server)
{
    server->sock = create_socket(SO_REUSE | SO_BIND | SO_NODELAY, (ipv4_t){0},
    args->port) LOG("Socket created")
    if (-1 == server->sock)
        return 1;
    server->max_client = server->sock;
    FD_ZERO(&server->current_socks);
    FD_SET(server->sock, &server->current_socks);
    if (1 == init_map(args, &server->map)) {
        ERROR("Map initialization failed")
        return close(server->sock), 1;
    }
    LOG("Map initialized")
    if (1 == init_teams(args, &server->teams)) {
        ERROR("Teams initialization failed")
        return free(server->map.tiles), close(server->sock), 1;
    }
    return 0;
}

/// @brief Function which destroy server (free everything).
/// @param server The server structure to destroy.
/// @return Returns 0.
static uint8_t destroy_server(const argument_t PTR args, const server_t
    *server)
{
    LOG("Server is closing")
    close(server->sock);
    free(server->map.tiles);
    destroy_teams(args, server->teams);
    LOG("Server destroyed")
    return 0;
}

uint8_t run_server(const argument_t *args)
{
    server_t server = {};

    if (1 == init_server(args, &server))
        return 84;
    return destroy_server(args, &server);
}
