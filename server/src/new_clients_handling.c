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
#include "queue/msg_queue.h"
#include "swap_clients.h"
#include "logging.h"
#include "events/new_client_events.h"

void init_new_client(server_t PTR server, new_client_t PTR client)
{
    msg_t message = {};

    server->nb_clients++;
    TAILQ_INIT(&client->queue);
    create_message("WELCOME\n", 8, &message);
    add_msg_to_queue(&client->queue, &message);
    client->expiration = server->clock;
    add_to_clock(&client->expiration, AUTH_TIMEOUT_SEC, AUTH_TIMEOUT_NS);
}

void destroy_new_client(server_t PTR server, uint32_t client_idx,
    bool preserve_sock)
{
    LOGF("Destroying new client (client idx: %u)", client_idx)
    if (false == preserve_sock) {
        FD_CLR(server->clients[client_idx].sock, &server->current_socks);
        close(server->clients[client_idx].sock);
    }
    clear_msg_queue(&server->clients[client_idx].queue);
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
/// @return true : If the client is a GUI, false If not.
static bool new_client_is_a_gui(server_t PTR server,
    uint64_t team_name_length, uint32_t client_idx, char ARRAY buffer)
{
    msg_t message = {};

    if (team_name_length + 1 == sizeof(GUI_TEAM) - 1 &&
    0 == strncmp(buffer, GUI_TEAM, sizeof(GUI_TEAM) - 1)) {
        LOG("Start swapping new client to GUI");
        if (FAILURE == transform_new_client_to_gui(server, client_idx)) {
            ERROR("Failed to accept new client in GUI's array")
            server->clients[client_idx].expiration = server->clock;
            add_to_clock(&server->clients[client_idx].expiration,
            AUTH_TIMEOUT_SEC, AUTH_TIMEOUT_NS);
            create_message((char *)&(int32_t){-1}, sizeof(int32_t), &message);
            message.event.new_client_event = NEW_CLIENT_EVENT_VERIFY_GUI_SLOT;
            add_msg_to_queue(&server->clients[client_idx].queue, &message);
        }
        return true;
    }
    return false;
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
    int32_t team_index = get_team_index_by_name(server->teams,
    server->args->nb_of_teams, buffer, (uint32_t)team_name_length);
    msg_t message = {};

    if (-1 == team_index)
        return destroy_new_client(server, client_idx, false);
    if (FAILURE == transform_new_client_to_ai(server, client_idx,
    team_index)) {
        ERROR("Failed to accept new client in PLAYER's array")
        server->clients[client_idx].expiration = server->clock;
        add_to_clock(&server->clients[client_idx].expiration,
        AUTH_TIMEOUT_SEC, AUTH_TIMEOUT_NS);
        create_message((char *)&team_index, sizeof(int32_t), &message);
        message.event.new_client_event = NEW_CLIENT_EVENT_VERIFY_AI_SLOT;
        add_msg_to_queue(&server->clients[client_idx].queue, &message);
    }
}

/// @brief Function called when the server receive data from a new_client_t.\n
/// - Put the client's message into a buffer.\n
/// - If EOF reached, destroy the client.\n
/// - Else, find out if the requested team exists.\n
/// - If the team doesn't exist, destroy the client.\n
/// - If the team exists, respond "ok\\n" to the client.
/// @param server The server pointer.
/// @param client The client index of the client who sent the message.
static void on_new_client_rcv(server_t PTR server, uint32_t client_idx)
{
    static char buffer[65];
    int64_t bytes_received =
        read(server->clients[client_idx].sock, buffer, sizeof(buffer) - 1);
    uint64_t team_name_length;

    if (bytes_received < 1) {
        LOG("Client closed connection")
        return destroy_new_client(server, client_idx, false);
    }
    LOGF("Client received : %.*s", (int32_t)bytes_received, buffer)
    buffer[bytes_received] = '\0';
    team_name_length = strcspn(buffer, "\n");
    if (true == new_client_is_a_gui(server, team_name_length, client_idx,
    buffer))
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
        if (true == is_timeout_exceed(&server->clock, &client->expiration))
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
    LOG("Start handle client rfds");
    switch (is_ready(server, &server->clients[client_idx], rfds)) {
        case 0:
            on_new_client_rcv(server, client_idx);
            (*select_ret)--;
            LOG("Stop handle client rfds 1")
            return 1;
        case 1:
            (*select_ret)--;
            LOG("Stop handle client rfds 2")
            return 1;
        default:
            LOG("Stop handle client rfds 3")
            return 0;
    }
}

/// @brief Function which pop message from the queue and send it.
/// @param client The current client.
static void send_next_message_from_queue(server_t PTR server,
    uint32_t client_idx)
{
    msg_t msg;

    if (FAILURE == pop_msg(&server->clients[client_idx].queue, &msg))
        return;
    if (false == execute_new_client_event_function(server, client_idx, &msg))
        return;
    LOGF("Send msg from queue (new client sock %i) : %.*s", client->sock, msg
    .len, msg.ptr)
    write(server->clients[client_idx].sock, msg.ptr, msg.len);
    destroy_message(&msg);
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
    LOG("Start handle client wfds");
    switch (is_ready(server, &server->clients[client_idx], wfds)) {
        case 0:
            send_next_message_from_queue(server, client_idx);
            (*select_ret)--;
            LOG("Stop handle client wfds 1")
            return 1;
        case 1:
            (*select_ret)--;
            LOG("Stop handle client wfds 2")
            return 1;
        default:
            LOG("Stop handle client wfds 3")
            return 0;
    }
}

void handle_new_clients(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    uint16_t count_client = 0;

    for (int32_t i = 0; count_client < server->nb_clients &&
    *select_ret > 0 && i < MAX_CLIENTS; i++) {
        LOGF("Handling the actual NEW CLIENT : %i", i)
        if (0 == server->clients[i].sock)
            continue;
        count_client++;
        if (1 == handle_client_rfds(server, i, rfds, select_ret))
            continue;
        if (1 == handle_client_wfds(server, i, wfds, select_ret))
            continue;
    }
}
