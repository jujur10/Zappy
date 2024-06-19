/*
** EPITECH PROJECT, 2024
** game_settings.h
** File description:
** Game parameters.
*/

#pragma once

/// @brief The delay for the map update in time units.
#define MAP_UPDATE_WAIT 20

/// @brief The cooldown delay for a forward action of player.
#define PLAYER_FORWARD_WAIT 7

/// @brief The cooldown delay for a turn right action of the player.
#define PLAYER_RIGHT_WAIT 7

/// @brief The cooldown delay for a turn left action of the player.
#define PLAYER_LEFT_WAIT 7

/// @brief The cooldown delay for a take action of the player.
#define PLAYER_TAKE_WAIT 7

/// @brief The cooldown delay for a set action of the player.
#define PLAYER_SET_WAIT 7

/// @brief The cooldown delay for the fork action.
#define PLAYER_FORK_WAIT 42

/// @brief The cooldown delay for the eject action.
#define PLAYER_EJECT_WAIT 7

/// @brief The cooldown delay for the inventory action.
#define PLAYER_INVENTORY_WAIT 1

/// @brief The cooldown delay for the inventory action.
#define PLAYER_LOOK_WAIT 7

/// @brief The cooldown for the incantation command.
#define PLAYER_INCANTATION_WAIT 300

/// @brief The cooldown delay for MCT in ns.
#define GUI_MCT_REQUEST_COOLDOWN 100000000

/// @brief The cooldown delay to re-eat food.
#define PLAYER_FOOD_CONSUMTION 126

/// @brief The maximal level of an AI. (8 by default).
#define MAX_AI_LVL 8

/// @brief The maximal number of a resources for a tile.
/// The value is needed in order to allocate the buffer at the program start.
#define MAX_RESOURCES_BY_TILE 10
