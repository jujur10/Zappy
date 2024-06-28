/*
** EPITECH PROJECT, 2024
** player_responses.c
** File description:
** player_responses.c.
*/
#include "server.h"

void player_ok_response(const server_t PTR server, player_t PTR player)
{
    queue_push_buffer_to_queue(&player->queue,
        &server->generated_buffers.buffers[PRE_OK_RESPONSE]);
}

void player_ko_response(const server_t PTR server, player_t PTR player)
{
    queue_push_buffer_to_queue(&player->queue,
        &server->generated_buffers.buffers[PRE_KO_RESPONSE]);
}
