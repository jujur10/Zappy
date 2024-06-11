/*
** EPITECH PROJECT, 2024
** player_commands.h
** File description:
** player_commands.h.
*/

#pragma once

#include <stdint.h>

#include "utils/string/string.h"

/// @brief Read buffer size in byte.
#define READ_BUFFER_SIZE 256

/// @brief Max number of reallocations.
#define MAX_TEMP_BUFFER_LEN 1024

/// @brief Redefinition of structures.
typedef struct player_s player_t;
typedef struct server_s server_t;

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Max number of commands into the buffer for PLAYER's.
#define MAX_NB_OF_COMMAND_FOR_BUFFER 10

/// @brief Enumeration representing the PLAYER's commands.
///
/// @var PLAYER_NONE_CMD Representing an empty slot.
/// @var PLAYER_MSZ_CMD Representing the MSZ command.
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
    PLAYER_DIRECTION_CMD,
} PACKED player_command_base_t;

/// @brief Macros representing the PLAYER's commands as text.
#define PLAYER_FORWARD_TXT "Forward\n"
#define PLAYER_RIGHT_TXT "Right\n"
#define PLAYER_LEFT_TXT "Left\n"
#define PLAYER_LOOK_TXT "Look\n"
#define PLAYER_INVENTORY_TXT "Inventory\n"
#define PLAYER_BROADCAST_TXT "Broadcast"
#define PLAYER_CONNECT_NB_TXT "Connect_nbr\n"
#define PLAYER_FORK_TXT "Fork\n"
#define PLAYER_EJECT_TXT "Eject\n"
#define PLAYER_TAKE_OBJ_TXT "Take"
#define PLAYER_SET_OBJ_TXT "Set"
#define PLAYER_INCANTATION_TXT "Incantation\n"
#define PLAYER_FREQUENCY_TXT "Frequency\n"
#define PLAYER_DIRECTION_TXT "Direction\n"

/// @brief Structure representing a PLAYER command.
///
/// @var command The command type.
/// @var argument The command argument.
typedef struct player_command_s {
    player_command_base_t command;
    string_t argument;
} player_command_t;

/// @brief Structure representing the PLAYER's command buffer.
///
/// @var raw_buffer The raw message buffer in order to stock partial
/// receptions (reception not '\n' terminated).
/// @var nb_of_command The number of commands actually in the 'commands' array.
/// @var commands The stocked commands.
typedef struct player_command_buffer_s {
    string_t raw_buffer;
    uint8_t nb_of_command;
    player_command_t commands[MAX_NB_OF_COMMAND_FOR_BUFFER];
} player_command_buffer_t;

/// @brief Function which effectuate message parsing.
///
/// @param server The server structure.
/// @param buffer The received buffer.
/// @param len The length of the received buffer.
/// @param player_index The index of the player in the player array.
void player_command_handling(server_t PTR server, char ARRAY buffer,
    uint32_t len, uint32_t player_index);
