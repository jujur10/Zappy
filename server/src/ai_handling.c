/*
** EPITECH PROJECT, 2024
** ai_handling.c
** File description:
** ai_handling.c.
*/
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "clock.h"
#include "queue/msg_queue.h"
#include "logging.h"
#include "commands/player_commands.h"

/// @brief Function which adds a player to a team (if possible).
///
/// @param server The server structure.
/// @param team_idx The team we want to assign the player to.
/// @param player_idx The player to add to the team.
/// @return SUCCESS if the player has been added successfully, FAILURE if not.
static status_t add_player_to_team(server_t PTR server, uint16_t team_idx,
    uint16_t player_idx)
{
    if (0 == get_nb_of_unused_slot(&server->teams[team_idx]))
        return FAILURE;
    server->teams[team_idx].nb_of_eggs--;
    server->teams[team_idx].players_idx[server->teams[team_idx]
        .nb_of_players] = player_idx;
    server->teams[team_idx].nb_of_players++;
    server->players[player_idx].team_idx = team_idx;
    return SUCCESS;
}

int32_t init_ai(server_t PTR server, int sock, uint16_t team_idx)
{
    player_t *player = &server->players[server->nb_players];

    if (MAX_CLIENTS == server->nb_players ||
    FAILURE == add_player_to_team(server, team_idx, server->nb_players))
        return -1;
    LOGF("Swapped to AI %i", server->nb_players)
    player->sock = (uint16_t)sock;
    TAILQ_INIT(&player->queue);
    player->time_to_live = LIFE_UNITS_TO_TIME_UNITS(BEGINNING_LIFE_UNITS);
    player->team_idx = team_idx;
    server->nb_players++;
    return server->nb_players - 1;
}

void destroy_ai(server_t PTR server, uint32_t ai_idx)
{
    LOGF("Destroying player (player idx: %u)", ai_idx)
    FD_CLR(server->players[ai_idx].sock, &server->current_socks);
    close(server->players[ai_idx].sock);
    clear_msg_queue(&server->players[ai_idx].queue);
    server->nb_players--;
    memmove(&server->players[ai_idx], &server->players[server->nb_players],
    sizeof(player_t));
    memset(&server->players[server->nb_players], 0, sizeof(player_t));
}

/// @brief Function called when the server receive data from a player_t.\n
/// - Put the player's message into a buffer.\n
/// - If EOF reached, destroy the player.\n
/// - Else, execute the actions ordered by the player.
/// @param server The server pointer.
/// @param player_idx The player index of the player who sent the message.
static void on_ai_rcv(server_t PTR server, uint32_t player_idx)
{
    char buffer[READ_BUFFER_SIZE];
    int64_t bytes_received = read(server->players[player_idx].sock, buffer,
    sizeof(buffer) - 1);

    if (bytes_received < 1) {
        LOG("Player closed connection")
        return destroy_ai(server, player_idx);
    }
    LOGF("Player received : %.*s", (int32_t)bytes_received, buffer)
    player_command_handling(server, buffer, (uint32_t)bytes_received,
        player_idx);
}

/// @brief Check the status of the player.
/// @param server The server pointer.
/// @param player The player we want to check the status.
/// @param fd_set The file descriptor set to check if the player is inside.
/// @return 0 : If the player is inside the fd set but the blocking time has
/// not expired.\n
/// 1 : If the player is inside the fd set and blocking time has expired.\n
/// 2 : If the player isn't inside the fd set.
static uint8_t is_ready(server_t PTR server, player_t PTR player,
    const fd_set PTR fd_set)
{
    if (FD_ISSET(player->sock, fd_set)) {
        if (false == has_blocking_time_expired(server->time_units,
        player->blocking_time))
            return 0;
        return 1;
    }
    return 2;
}

static void blocking_time_not_respected(server_t PTR server,
    uint32_t player_idx, int32_t PTR select_ret)
{
    msg_t message;
    char buffer[10];
    uint64_t bytes_received = read(server->players[player_idx].sock, buffer,
        sizeof(buffer));

    if (bytes_received < 1) {
        LOG("Player closed connection")
        return destroy_ai(server, player_idx);
    }
    create_message("ko\n", 4, &message);
    add_msg_to_queue(&server->players[player_idx].queue, &message);
    (*select_ret)--;
}

/// @brief Function which check and handle if the player's socket waiting to
/// be read.
/// @param server The server pointer.
/// @param player_idx The index of the player we want to check.
/// @param rfds The read file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_players_rfds(server_t PTR server, uint32_t player_idx,
    const fd_set PTR rfds, int32_t PTR select_ret)
{
    LOG("Start handle PLAYER rfds");
    switch (is_ready(server, &server->players[player_idx], rfds)) {
        case 0:
            blocking_time_not_respected(server, player_idx, select_ret);
            LOG("Stop handle PLAYER rfds 1")
            return 1;
        case 1:
            on_ai_rcv(server, player_idx);
            (*select_ret)--;
            LOG("Stop handle PLAYER rfds 2")
            return 1;
        default:
            LOG("Stop handle PLAYER rfds 3")
            return 0;
    }
}

/// @brief Function which pop message from the queue and send it.
/// @param player The current player.
static void send_next_message_from_queue(player_t PTR player)
{
    msg_t msg;

    if (FAILURE == pop_msg(&player->queue, &msg))
        return;
    LOGF("Send msg from queue (PLAYER sock %hu) : %.*s", player->sock, msg
    .len, msg.ptr)
    write(player->sock, msg.ptr, msg.len);
    destroy_message(&msg);
}

/// @brief Function which check and handle if the player's socket waiting to be
/// write.
/// @param server The server pointer.
/// @param player_idx The index of the player we want to check.
/// @param wfds The write file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_players_wfds(server_t PTR server, uint32_t player_idx,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    LOG("Start handle PLAYER wfds")
    switch (is_ready(server, &server->players[player_idx], wfds)) {
        case 0:
        case 1:
            send_next_message_from_queue(&server->players[player_idx]);
            (*select_ret)--;
            LOG("Stop handle PLAYER wfds 1")
            return 1;
        default:
            LOG("Stop handle PLAYER wfds 2")
            return 0;
    }
}

void handle_players(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    uint16_t count_players = 0;

    LOG("Start handling PLAYERS");
    for (int32_t i = 0; count_players < server->nb_players && *select_ret > 0
        && i < MAX_CLIENTS; i++) {
        LOGF("Actual PLAYER %i", i)
        if (0 == server->players[i].sock)
            continue;
        count_players++;
        if (FAILURE == queue_empty(&server->players[i].queue)) {
            handle_players_wfds(server, i, wfds, select_ret);
            continue;
        }
        if (1 == handle_players_rfds(server, i, rfds, select_ret))
            continue;
        if (1 == handle_players_wfds(server, i, wfds, select_ret))
            continue;
    }
    LOG("Stop handling PLAYERS")
}
