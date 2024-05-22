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
#include "queue.h"

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Enumeration representing the status of the player.
///
/// @var PLAYING If the player is playing or idling.
/// @var INTO_RITUAL If the player is into a ritual.
typedef enum status_s {
    PLAYING,
    INTO_RITUAL
} PACKED status_t;

/// @brief Structure representing player.
///
/// @var sock The player's socket.
/// @var level The player's level (correlated to player's field of vision).
/// @var orientation The orientation of the player.
/// @var coordinates The coordinates of the player on the map.
/// @var inventory The inventory of the player.
/// @var time_to_live The remaining time to live for the player.
/// @var blocking_time Representing the timestamp at which the player can
/// execute an action again.
/// @var status The current player's status.
/// @var queue The player's receiving message queue.
typedef struct player_s {
    int sock;
    uint8_t level;
    orientation_t orientation;
    coordinates_t coordinates;
    inventory_t inventory;
    uint32_t time_to_live;
    uint32_t blocking_time;
    status_t status;
    shared_message_queue_head_t queue;
} player_t;
