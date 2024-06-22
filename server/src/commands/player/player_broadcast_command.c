/*
** EPITECH PROJECT, 2024
** player_broadcast_command.c
** File description:
** player_broadcast_command.c.
*/
#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"
#include "game_settings.h"

/// @brief There is always 8 cells around a point in a 2d array.
static const int32_t NB_OF_CELLS_AROUND_CENTER = 8;

/// @brief Function used to returns the angle's corresponding index.
///
/// @param angle The angle we want to obtain the index.
/// @return The index.
static int32_t get_index_from_angle(double angle)
{
    int32_t index = (int32_t)((angle + 22.5) / 45.0);

    return (8 == index) ? 0 : index;
}

/// @brief Function used to calculate the shortest distance and put the
/// distance (possibly negative) into shortest_distance.
///
/// @param map The map.
/// @param current_player The player coordinates sending the message.
/// @param receiving_player The player coordinates receiving the message.
/// @param shortest_distance The shortest distance.
static void get_shortest_distance(const map_t PTR map,
    const coordinates_t PTR current_player,
    const coordinates_t PTR receiving_player,
    coordinates_i32_t PTR shortest_distance)
{
    coordinates_i32_t distance;
    coordinates_i32_t absolute_distance;

    get_vec_distance(receiving_player, current_player, &distance);
    absolute_distance.x = ABSOLUTE(distance.x);
    absolute_distance.y = ABSOLUTE(distance.y);
    if (absolute_distance.x > (map->width / 2)) {
        shortest_distance->x = (distance.x > 0) ? distance.x - map->width :
            distance.x + map->width;
    } else {
        shortest_distance->x = distance.x;
    }
    if (absolute_distance.y > (map->height / 2)) {
        shortest_distance->y = (distance.y > 0) ? distance.y - map->height :
            distance.y + map->height;
    } else {
        shortest_distance->y = distance.y;
    }
}

/// @brief Function used to calculate the direction of the shortest distance.
///
/// @param map The map.
/// @param current_player The player sending the message.
/// @param receiving_player The player receiving the message.
static int32_t get_shortest_direction(const map_t PTR map,
    const player_t PTR current_player,
    const player_t PTR receiving_player)
{
    coordinates_i32_t shortest_distance;
    double angle;
    int32_t direction;

    get_shortest_distance(map, &current_player->coordinates,
        &receiving_player->coordinates, &shortest_distance);
    angle = RADIAN_TO_DEGREE(NORMALIZE_RAD(-atan2(shortest_distance.y,
        shortest_distance.x)));
    direction = (get_index_from_angle(angle)
        + (((receiving_player->orientation - 1 + NB_OF_ORIENTATION)
        % NB_OF_ORIENTATION) * 2)) % NB_OF_CELLS_AROUND_CENTER + 1;
    return direction;
}

/// @brief Function used to broadcast the message after the direction
/// computation.
///
/// @param player The player.
/// @param message_buffer The message to broadcast (as a buffer to copy).
/// @param direction The direction of the sender.
static void broadcast_message_to_player(const server_t PTR server,
    player_t PTR player, const buffer_t PTR message_buffer, int32_t direction)
{
    msg_t message = {};

    create_message(message_buffer->ptr, message_buffer->len,
        &message);
    fast_itoa_u32(direction, message.ptr + 8);
    add_msg_to_queue(&player->queue, &message);
    add_time_limit_to_player(server->time_units, PLAYER_BROADCAST_WAIT,
        player);
}

/// @brief Function which re-initialize the broadcast buffer.
/// - Reinitialize.
/// - Append the message.
///
/// @param buffer The buffer to re-initialize.
/// @param command The player's command (containing the message).
static void reinitialize_broadcast_buffer(buffer_t PTR buffer,
    const player_command_t PTR command)
{
    buffer->len = sizeof(MESSAGE_STR) - 1;
    append_to_buffer_from_chars(buffer, command->argument.ptr,
        command->argument.len);
    append_to_buffer_from_chars(buffer, "\n", 1);
    free(command->argument.ptr);
}

void execute_player_broadcast_command(server_t PTR server, uint16_t player_idx,
    const player_command_t PTR command)
{
    player_t *sender = &server->players[player_idx];
    player_t *receiver;
    const coordinates_t *sender_coo = &sender->coordinates;
    const coordinates_t *receiver_coo;
    int32_t direction;
    buffer_t *buffer = &server->generated_buffers.buffers[PRE_BROADCAST_MSG];

    reinitialize_broadcast_buffer(buffer, command);
    send_pbc_to_guis(server, buffer, player_idx);
    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (i == player_idx)
            continue;
        receiver = &server->players[i];
        receiver_coo = &receiver->coordinates;
        direction = (true == is_coordinates_equal(sender_coo, receiver_coo)) ?
            0 : get_shortest_direction(&server->map, sender, receiver);
        broadcast_message_to_player(server, receiver, buffer, direction);
    }
    player_ok_response(server, sender);
}
