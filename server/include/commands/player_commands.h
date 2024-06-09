/*
** EPITECH PROJECT, 2024
** player_commands.h
** File description:
** player_commands.h.
*/

#pragma once

#include <stdint.h>

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Max number of commands into the buffer for player.
#define MAX_NB_OF_COMMAND_FOR_BUFFER 10

/// @brief Enumeration representing the player's commands.
///
/// @var PLAYER_NONE_CMD Representing an empty slot.
/// @var PLAYER_FORWARD_CMD Representing the forward command.
/// @var PLAYER_RIGHT_CMD Representing the command used to turn right.
/// @var PLAYER_LEFT_CMD Representing the command used to turn left.
/// @var PLAYER_LOOK_CMD Representing the look command.
/// @var PLAYER_INVENTORY_CMD Representing the inventory command.
/// @var PLAYER_BROADCAST_CMD Representing the broadcast command.
/// @var PLAYER_CONNECT_NB_CMD Representing the connect command.
/// @var PLAYER_FORK_CMD Representing the fork command.
/// @var PLAYER_EJECT_CMD Representing the eject command.
/// @var PLAYER_TAKE_OBJ_CMD Representing the take object action.
/// @var PLAYER_SET_OBJ_CMD Representing the set object action.
/// @var PLAYER_INCANTATION_CMD Representing the incantation command.
/// @var PLAYER_FREQUENCY_CMD Representing the frequency command.
/// @var PLAYER_DIRECTION_CMD Representing the direction command.
typedef enum {
    PLAYER_NONE_CMD,
    PLAYER_FORWARD_CMD,
    PLAYER_RIGHT_CMD,
    PLAYER_LEFT_CMD,
    PLAYER_LOOK_CMD,
    PLAYER_INVENTORY_CMD,
    PLAYER_BROADCAST_CMD,
    PLAYER_CONNECT_NB_CMD,
    PLAYER_FORK_CMD,
    PLAYER_EJECT_CMD,
    PLAYER_TAKE_OBJ_CMD,
    PLAYER_SET_OBJ_CMD,
    PLAYER_INCANTATION_CMD,
    PLAYER_FREQUENCY_CMD,
    PLAYER_DIRECTION_CMD
} PACKED player_command_base_t;

/// @brief Structure representing a player command.
///
/// @var command The command type.
/// @var argument The command argument (in case of broadcast command for ex).
typedef struct player_command_s {
    player_command_base_t command;
    char ARRAY argument;
} player_command_t;

/// @brief Structure representing the player's command buffer.
///
/// @var raw_buffer The raw message buffer in order to stock partial
/// receptions (reception not '\n' terminated).
/// @var nb_of_command The number of commands actually in the 'commands' array.
/// @var commands The stocked commands.
typedef struct player_command_buffer_s {
    char ARRAY raw_buffer;
    uint8_t nb_of_command;
    player_command_t commands[MAX_NB_OF_COMMAND_FOR_BUFFER];
} player_command_buffer_t;
