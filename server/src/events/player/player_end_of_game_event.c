/*
** EPITECH PROJECT, 2024
** player_end_of_game_event.c
** File description:
** player_end_of_game_event.c.
*/
#include <stdint.h>

#include "server.h"
#include "ai_handling.h"

void send_end_of_game_to_players(server_t PTR server)
{
    msg_t message = {};

    set_event_to_message(&message, (event_t){PLAYER_EVENT_END_OF_GAME,
        GUI_EVENT_NONE, NEW_CLIENT_EVENT_NONE});
    for (uint32_t i = 0; i < server->nb_players; i++)
        queue_push(&server->players[i].queue, &message);
}

void execute_player_end_of_game_event(server_t PTR server, uint32_t player_idx)
{
    destroy_ai(server, player_idx);
}
