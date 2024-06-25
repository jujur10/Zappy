/*
** EPITECH PROJECT, 2024
** player_death_event.c
** File description:
** player_death_event.c.
*/
#include "ai_handling.h"
#include "events/player_events.h"
#include "utils/itoa/fast_itoa.h"

void create_player_death_event(player_t PTR player)
{
    msg_t message = {};

    create_message("dead\n", 5, &message);
    message.event.player_event = PLAYER_EVENT_KILL;
    add_msg_to_queue(&player->queue, &message);
}

void execute_player_death_event(server_t PTR server, uint32_t player_idx)
{
    send_pdi_to_guis(server, player_idx);
    destroy_ai(server, player_idx);
}
