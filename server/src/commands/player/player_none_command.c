/*
** EPITECH PROJECT, 2024
** player_none_command.c
** File description:
** player_none_command.c.
*/
#include "server.h"

#include "queue/msg_queue.h"

void execute_player_none_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];

    add_buffer_to_queue(&player->queue, &server->generated_buffers
    .buffers[PRE_KO_RESPONSE]);
}
