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

// Todo: complete this function and modify clock in order to set the clock
//  just before the next select.
void init_new_client(server_t PTR server, new_client_t PTR client)
{
    server->nb_clients++;
    send(client->sock, "WELCOME\n", 8, 0);
    clock_gettime(CLOCK_MONOTONIC, &client->expiration);
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

    if (-1 == bytes_received)
        destroy_new_client(server, client);
    client_name_length = strcspn(buffer, "\n");
    team_index = get_team_index_by_name(server->teams,
    server->args->nb_of_teams, buffer, (uint32_t)client_name_length);
    if (-1 == team_index)
        destroy_new_client(server, client);
    send(client->sock, "ok\n", 3, 0);
}

void handle_new_clients(server_t *server, const fd_set *rfds,
    const fd_set *wfds, int32_t *select_ret)
{
    uint16_t count_client = 0;

    for (int32_t i = 0; count_client < server->nb_clients && *select_ret > 0
    && i < MAX_CLIENTS; i++) {
        if (0 == server->clients[i].sock)
            continue;
        if (FD_ISSET(server->clients[i].sock, rfds)) {
            on_new_client_rcv(server, &server->clients[i]);
            count_client++;
            (*select_ret)--;
            continue;
        }
        if (FD_ISSET(server->clients[i].sock, wfds)) {
            count_client++;
            (*select_ret)--;
            continue;
        }
    }
}
