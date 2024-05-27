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

/// @brief Function which initializes map.
/// @param args The parsed program parameters.
/// @param map The map to initialize.
/// @return 0 on success, 1 on failure.
static uint8_t init_map(const argument_t *args, map_t *map)
{
    map->tiles = malloc(sizeof(resources_t) * (args->height * args->width));
    if (NULL == map->tiles)
        return 1;
    map->height = args->height;
    map->width = args->width;
    map->has_been_modified = true;
    return 0;
}

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
    return 0;
}

/// @brief Function which destroy server (free everything).
/// @param server The server structure to destroy.
/// @return Returns 0.
static uint8_t destroy_server(const server_t *server)
{
    LOG("Server is closing")
    close(server->sock);
    free(server->map.tiles);
    LOG("Server destroyed")
    return 0;
}

uint8_t run_server(const argument_t *args)
{
    server_t server = {};

    if (1 == init_server(args, &server))
        return 84;
    return destroy_server(&server);
}
