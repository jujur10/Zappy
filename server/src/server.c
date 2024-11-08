/*
** EPITECH PROJECT, 2024
** server.c
** File description:
** server.c.
*/
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include "socket/sockets.h"
#include "server.h"
#include "arguments.h"
#include "logging.h"
#include "team.h"
#include "map.h"
#include "signal_handler.h"
#include "new_clients_handling.h"
#include "gui_handling.h"
#include "ai_handling.h"
#include "clock.h"
#include "utils/pre_generate/pre_generate.h"
#include "server_update.h"

// Initialization of pipe_signals inside init_sig_pipe function.
int pipe_signals[2];

void send_quit_server_request(void)
{
    if (-1 != pipe_signals[1]) {
        write(pipe_signals[1], "TERMINATED", 10);
        close(pipe_signals[1]);
        pipe_signals[1] = -1;
    }
}

/// @brief Function which initializes signal pipe.
/// @param args The parsed program parameters.
/// @param server The server structure to initialize.
/// @return 0 on success, 1 on failure.
static uint8_t init_sig_pipe(const argument_t PTR args, server_t PTR server)
{
    if (-1 == pipe(pipe_signals)) {
        ERROR("Signal pipe initialization failed")
        free(server->map.tiles);
        destroy_teams(args, server->teams);
        close(server->sock);
        return 1;
    }
    FD_SET(pipe_signals[0], &server->current_socks);
    fcntl(pipe_signals[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe_signals[1], F_SETFL, O_NONBLOCK);
    return 0;
}

/// @brief Function which initializes server.\n
/// - Creating socket.\n
/// - Initialize map.\n
/// - Initialize teams.\n
/// - Initialize the signal pipe.
/// @param args The parsed program parameters.
/// @param server The server structure to initialize.
/// @return 0 on success, 1 on failure.
static uint8_t init_server(const argument_t PTR args, server_t PTR server)
{
    server->sock = create_socket(SO_REUSE | SO_BIND, (ipv4_t){0},
    args->port) LOG("Socket created")
    if (-1 == server->sock)
        return 1;
    server->max_client = server->sock;
    FD_ZERO(&server->current_socks);
    FD_SET(server->sock, &server->current_socks);
    if (1 == init_map(args, &server->map)) {
        ERROR("Map initialization failed") return close(server->sock), 1;
    }
    LOG("Map initialized")
    if (1 == init_teams(args, &server->teams, &server->egg_counter)) {
        ERROR("Teams initialization failed")
        return free(server->map.tiles), close(server->sock), 1;
    }
    FD_ZERO(&server->current_socks);
    FD_SET(server->sock, &server->current_socks);
    if (1 == init_sig_pipe(args, server))
        return 1;
    return 0;
}

/// @brief Function which destroy server (free everything).
/// - Close every connected sockets.\n
/// - Close server's socket.\n
/// - Free map.\n
/// - Free teams.
/// @param args The parsed program parameters.
/// @param server The server structure to destroy.
/// @return Returns 0.
static uint8_t destroy_server(const argument_t PTR args, server_t PTR server)
{
    LOG("Server closing")
    destroy_pre_generated_buffers(&server->generated_buffers);
    for (uint16_t i = 0; i < server->nb_clients; i++)
        destroy_new_client(server, i, false);
    for (uint16_t i = 0; i < server->nb_players; i++)
        destroy_ai(server, i);
    for (uint16_t i = 0; i < server->nb_guis; i++)
        destroy_gui(server, i);
    close(server->sock);
    free(server->map.tiles);
    destroy_teams(args, server->teams);
    LOG("Server destroyed")
    return 0;
}

/// @brief Function that initialize the file descriptor sets.
/// @param original The server fd set to copy.
/// @param read_fds The read fd set to update.
/// @param write_fds The write fd set to update.
static void init_fdset(const fd_set PTR original, fd_set PTR read_fds,
    fd_set *write_fds)
{
    *read_fds = *original;
    *write_fds = *original;
}

/// @brief Function which handle select function errors.
/// @return Returns 0, if the error was a signal (excepted), else returns 84
///  if not excepted.
static uint8_t select_error(void)
{
    return (EINTR == errno) ? 0 : 84;
}

/// @brief Function which handle every sockets (new clients, players and GUIs).
///
/// @param server The server structure.
/// @param rfds The read file descriptor set.
/// @param wfds The write file descriptor set.
/// @param select_ret The select returns value.
static void handle_sockets(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    if (FD_ISSET(server->sock, rfds))
        on_connection(server);
    else {
        handle_new_clients(server, rfds, wfds, select_ret);
        handle_players(server, rfds, wfds, select_ret);
        handle_guis(server, rfds, wfds, select_ret);
    }
}

/// @brief Function which runs the server main's loop.
/// @param server The server structure.
/// @return Returns 0 when the server quited properly, 84 if not (undefined
///  behavior).
static uint8_t server_main_loop(server_t PTR server)
{
    fd_set rfds;
    fd_set wfds;
    int32_t select_ret;
    struct timeval select_timeout = {0, 100000};

    while (true) {
        init_fdset(&server->current_socks, &rfds, &wfds);
        select_ret = select(server->max_client + 1, &rfds, &wfds, NULL,
            &select_timeout);
        if (-1 == select_ret)
            return select_error();
        update_server(server);
        if (FD_ISSET(pipe_signals[0], &rfds))
            return close(pipe_signals[0]), 0;
        handle_sockets(server, &rfds, &wfds, &select_ret);
    }
}

uint8_t run_server(const argument_t PTR args)
{
    server_t server = {};
    uint8_t ret_val;

    if (1 == init_server(args, &server))
        return 84;
    register_signals();
    srand((uint32_t)time(NULL));
    server.args = args;
    server.frequency = args->frequency;
    if (FAILURE == pre_generate_buffers(&server))
        return 84;
    spread_resources_on_map(&server.map, &server.generated_buffers);
    update_map_buffer(&server.map, &server.generated_buffers
        .buffers[PRE_MAP_BUFFER]);
    LOG("Server responses pre-generated")
    ret_val = server_main_loop(&server);
    destroy_server(args, &server);
    return ret_val;
}
