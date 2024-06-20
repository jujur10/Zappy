/*
** EPITECH PROJECT, 2024
** player_death_event.c
** File description:
** player_death_event.c.
*/
#include "ai_handling.h"
#include "events/player_events.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

void create_player_death_event(player_t PTR player)
{
    msg_t message = {};

    create_message("dead\n", 5, &message);
    message.event.player_event = PLAYER_EVENT_KILL;
    add_msg_to_queue(&player->queue, &message);
}

/// @brief Function which sends to GUIs the events of pdi.
///
/// @param server The server structure.
/// @param player_idx The player index.
static void send_pdi_to_guis(server_t PTR server, uint32_t player_idx)
{
    char msg_content[4 + UINT32_MAX_DIGITS + 1] = "pdi ";
    uint32_t count = 4;

    write_nb_to_buffer(server->players[player_idx].sock, msg_content, &count);
    msg_content[count - 1] = '\n';
    send_message_to_guis(server, msg_content, count);
}

void execute_player_death_event(server_t PTR server, uint32_t player_idx)
{
    send_pdi_to_guis(server, player_idx);
    return destroy_ai(server, player_idx);
}
