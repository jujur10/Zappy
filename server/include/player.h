/*
** EPITECH PROJECT, 2024
** player.h
** File description:
** player.h.
*/

#pragma once

#include <stdint.h>

#include "coordinates.h"
#include "inventory.h"
#include "utils/queue/queue.h"
#include "commands/player_commands.h"
#include "style/macros.h"

/// @brief Defaults parameters.
#define BEGINNING_LIFE_UNITS 10

/// @brief Enumeration representing the status of the player.
///
/// @var PLAYING If the player is playing or idling.
/// @var INTO_RITUAL If the player is into a ritual.
typedef enum player_status_s {
    PLAYING,
    INTO_RITUAL
} PACKED player_status_t;

/// @brief Structure representing player.
///
/// @var sock The player's socket.
/// @var level The player's level (correlated to player's field of vision).
/// @var status The current player's status.
/// @var orientation The orientation of the player.
/// @var coordinates The coordinates of the player on the map.
/// @var inventory The inventory of the player.
/// @var time_to_live The remaining time to live for the player.
/// @var blocking_time Representing the time at which the player can execute
/// an action again.
/// @var command_buffer The player's command buffer.
/// @var queue The player's receiving message queue.
typedef struct player_s {
    uint16_t sock;
    uint8_t level;
    uint16_t team_idx;
    player_status_t status;
    orientation_t orientation;
    coordinates_t coordinates;
    inventory_t inventory;
    double time_to_eat;
    double blocking_time;
    player_command_buffer_t command_buffer;
    queue_t queue;
} player_t;

/// @brief Function which returns the index of the player for a given socket.
///
/// @param server The server structure.
/// @param sock The socket of the player.
/// @return The index of the player or -1 if not found.
int32_t get_player_by_socket(const server_t PTR server, uint16_t sock);

/// @brief Function which adds the time limit to the player.
///
/// @param server_time_units The server current time units counter.
/// @param time_limit The time limit to set.
/// @param player The player we want to add the time limit to.
void add_time_limit_to_player(double server_time_units,
    uint32_t time_limit, player_t PTR player);

/// @brief Function which sets the next coordinates of the player in
/// new_coordinates.
///
/// @param map The map of the game.
/// @param player The concerned player.
/// @param new_coordinates A pointer on a coordinate structure to overwrite.
void get_next_player_coordinates(const map_t PTR map,
    const player_t PTR player, coordinates_t PTR new_coordinates);

/// @brief Function used to teleport a player to given coordinates.
///
/// @param map The player's map.
/// @param player The player we want to teleport.
/// @param new_coordinates The target coordinates.
void teleport_player(map_t PTR map, player_t PTR player,
    const coordinates_t PTR new_coordinates);

/// @brief Function used to take or set resource to tile.
///
/// @param player The player.
/// @param tile The current tile.
/// @param resource_index The resource index.
/// @param action The action (PLAYER_SET_OBJ_CMD or PLAYER_TAKE_OBJ_CMD).
void player_take_set_to_tile(player_t PTR player, resources_t PTR tile,
    resources_index_t resource_index, player_command_base_t action);
