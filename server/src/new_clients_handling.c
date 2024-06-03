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
#include "new_clients_handling.h"
#include "utils/itoa/fast_itoa.h"

void init_new_client(server_t PTR server, new_client_t PTR client)
{
    server->nb_clients++;
    send(client->sock, "WELCOME\n", 8, 0);
    client->expiration = server->clock;
    add_to_clock(&client->expiration, AUTH_TIMEOUT_SEC,
    AUTH_TIMEOUT_NS);
}

void destroy_new_client(server_t PTR server, uint32_t client_idx)
{
    FD_CLR(server->clients[client_idx].sock, &server->current_socks);
    close(server->clients[client_idx].sock);
    server->nb_clients--;
    memmove(&server->clients[client_idx], &server->clients[server->nb_clients],
    sizeof(new_client_t));
    memset(&server->clients[server->nb_clients], 0, sizeof(new_client_t));
}

/// @brief Function executed when the new client is a GUI.
/// @param server The server pointer.
/// @param team_name_length The team name length.
/// @param client_idx The client index of the new client array.
/// @param buffer The buffer containing the original message (and overwrite
/// by the response).
/// @return 0 : If the client is a GUI, 1 If not.
static uint8_t new_client_is_a_gui(server_t PTR server,
    uint64_t team_name_length, uint32_t client_idx, char ARRAY buffer)
{
    uint64_t msg_length;

    if (team_name_length + 1 == sizeof(GUI_TEAM) - 1 &&
    0 == strncmp(buffer, GUI_TEAM, sizeof(GUI_TEAM) - 1)) {
        msg_length = fast_itoa_u32(MAX_CLIENTS - server->nb_guis, buffer);
        memcpy(buffer + msg_length, "\n\0", 2);
        send(server->clients[client_idx].sock, buffer, msg_length + 2, 0);
        return 0;
    }
    return 1;
}

/// @brief Function executed when the new client is an AI.
/// @param server The server pointer.
/// @param team_name_length The team name length.
/// @param client_idx The client index of the new client array.
/// @param buffer The buffer containing the original message (and overwrite
/// by the response).
static void new_client_is_an_ai(server_t PTR server,
    uint64_t team_name_length, uint32_t client_idx, char ARRAY buffer)
{
    uint64_t msg_length;
    int32_t team_index = get_team_index_by_name(server->teams,
    server->args->nb_of_teams, buffer, (uint32_t)team_name_length);

    if (-1 == team_index)
        destroy_new_client(server, client_idx);
    msg_length = fast_itoa_u32(server->teams[team_index].max_nb_of_players -
    server->teams[team_index].nb_of_players, buffer);
    memcpy(buffer + msg_length, "\n\0", 2);
    send(server->clients[client_idx].sock, buffer, msg_length + 2, 0);
}

void on_new_client_rcv(server_t PTR server, uint32_t client_idx)
{
    static char buffer[64];
    int64_t bytes_received = recv(server->clients[client_idx].sock, buffer,
    sizeof(buffer), 0);
    uint64_t team_name_length;

    if (bytes_received < 1)
        destroy_new_client(server, client_idx);
    team_name_length = strcspn(buffer, "\n");
    if (0 == new_client_is_a_gui(server, team_name_length, client_idx, buffer))
        return;
    new_client_is_an_ai(server, team_name_length, client_idx, buffer);
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
/// @param client_idx The index of the client we want to check.
/// @param rfds The read file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_client_rfds(server_t PTR server, uint32_t client_idx,
    const fd_set PTR rfds, int32_t PTR select_ret)
{
    switch (is_ready(server, &server->clients[client_idx], rfds)) {
        case 0:
            destroy_new_client(server, client_idx);
            (*select_ret)--;
            return 1;
        case 1:
            on_new_client_rcv(server, client_idx);
            (*select_ret)--;
            return 1;
        default:
            return 0;
    }
}

/// @brief Function which check and handle if the client's socket waiting to
/// be write.
/// @param server The server pointer.
/// @param client_idx The index of the client we want to check.
/// @param wfds The write file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_client_wfds(server_t PTR server, uint32_t client_idx,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    switch (is_ready(server, &server->clients[client_idx], wfds)) {
        case 0:
            destroy_new_client(server, client_idx);
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
        if (1 == handle_client_rfds(server, i, rfds,
        select_ret))
            continue;
        if (1 == handle_client_wfds(server, i, wfds, select_ret))
            continue;
    }
}
