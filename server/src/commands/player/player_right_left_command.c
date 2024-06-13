/*
** EPITECH PROJECT, 2024
** player_right_left_command.c
** File description:
** player_right_left_command.c.
*/
#include "server.h"

#include "queue/msg_queue.h"
#include "game_settings.h"

/// @brief Function which modify the player's orientation with a rotation.
///
/// @param player The player to modify.
/// @param rotation The rotation to effectuate (LOOK_RIGHT or LOOK_LEFT).
void execute_player_rotation(player_t *player, orientation_t rotation)
{
    orientation_t *player_orientation = &player->orientation;

    *player_orientation = (*player_orientation + (rotation == LOOK_RIGHT) -
        (rotation == LOOK_LEFT) + NB_OF_ORIENTATION) % NB_OF_ORIENTATION;
}

void execute_player_right_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];

    execute_player_rotation(player, LOOK_RIGHT);
    add_buffer_to_queue(&player->queue, &server->generated_buffers
    .buffers[PRE_OK_RESPONSE]);
    add_time_limit_to_player(server->time_units, PLAYER_RIGHT_WAIT, player);
}

void execute_player_left_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];

    execute_player_rotation(player, LOOK_LEFT);
    add_buffer_to_queue(&player->queue, &server->generated_buffers
    .buffers[PRE_OK_RESPONSE]);
    add_time_limit_to_player(server->time_units, PLAYER_LEFT_WAIT, player);
}
