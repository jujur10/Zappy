/*
** EPITECH PROJECT, 2024
** new_clients.c
** File description:
** new_clients.c.
*/
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "clock.h"

void init_new_client(server_t PTR server, new_client_t PTR client)
{
    server->nb_clients++;
    send(client->sock, "WELCOME\n", 8, 0);
    client->expiration = server->clock;
    add_to_clock(&client->expiration, AUTH_TIMEOUT_SEC,
    AUTH_TIMEOUT_NS);
}

void destroy_new_client(server_t PTR server, new_client_t PTR client)
{
    FD_CLR(client->sock, &server->current_socks);
    close(client->sock);
    memset(client, 0, sizeof(new_client_t));
    server->nb_clients--;
}

void on_new_client_rcv(server_t PTR server, new_client_t PTR client)
{
    static char buffer[64];
    int64_t bytes_received = recv(client->sock, buffer, sizeof(buffer), 0);
    uint64_t client_name_length;
    int32_t team_index;

    if (bytes_received < 1)
        destroy_new_client(server, client);
    client_name_length = strcspn(buffer, "\n");
    team_index = get_team_index_by_name(server->teams,
    server->args->nb_of_teams, buffer, (uint32_t)client_name_length);
    if (-1 == team_index)
        destroy_new_client(server, client);
    send(client->sock, "ok\n", 3, 0);
}

/// @brief Check the status of the client.
/// @param server The server pointer.
/// @param client The client we want to check the status.
/// @param fd_set The file descriptor set to check if the client is inside.
/// @return 0 : If the client is inside the fd set but timeout has exceed.\n
/// 1 : If the client is inside the fd set and timeout hasn't exceed.\n
/// 2 : If the client isn't inside the fd set.
static uint8_t is_ready(server_t PTR server, new_client_t PTR client,
    const fd_set PTR fd_set)
{
    if (FD_ISSET(client->sock, fd_set)) {
        if (0 == is_timeout_exceed(&server->clock,
        &client->expiration))
            return 0;
        return 1;
    }
    return 2;
}

/// @brief Function which check and handle if the client's socket waiting to
/// be read.
/// @param server The server pointer.
/// @param client The client we want to check.
/// @param rfds The read file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_client_rfds(server_t PTR server, new_client_t PTR client,
    const fd_set PTR rfds, int32_t PTR select_ret)
{
    switch (is_ready(server, client, rfds)) {
        case 0:
            destroy_new_client(server, client);
            (*select_ret)--;
            return 1;
        case 1:
            on_new_client_rcv(server, client);
            (*select_ret)--;
            return 1;
        default:
            return 0;
    }
}

/// @brief Function which check and handle if the client's socket waiting to
/// be write.
/// @param server The server pointer.
/// @param client The client we want to check.
/// @param wfds The write file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_client_wfds(server_t PTR server, new_client_t PTR client,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    switch (is_ready(server, client, wfds)) {
        case 0:
            destroy_new_client(server, client);
            (*select_ret)--;
            return 1;
        case 1:
            (*select_ret)--;
            return 1;
        default:
            return 0;
    }
}

void handle_new_clients(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    uint16_t count_client = 0;

    for (int32_t i = 0; count_client < server->nb_clients && *select_ret > 0
    && i < MAX_CLIENTS; i++) {
        if (0 == server->clients[i].sock)
            continue;
        count_client++;
        if (1 == handle_client_rfds(server, &server->clients[i], rfds,
        select_ret))
            continue;
        if (1 == handle_client_wfds(server, &server->clients[i], wfds,
        select_ret))
            continue;
    }
}
