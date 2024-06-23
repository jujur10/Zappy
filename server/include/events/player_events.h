/*
** EPITECH PROJECT, 2024
** player_events.h
** File description:
** player_events.h.
*/

#pragma once

/// @brief Redefinition.
typedef struct player_s player_t;
typedef struct server_s server_t;

/// @brief Enumeration representing the possible player event to handle.
///
/// @var PLAYER_EVENT_NONE Representing empty event.
/// @var PLAYER_EVENT_KILL Representing the death of a player.
/// @var PLAYER_NB_EVENT Representing the number of possible event.
typedef enum {
    PLAYER_EVENT_NONE,
    PLAYER_EVENT_KILL,
    PLAYER_EVENT_INCANTATION,
    PLAYER_NB_EVENT
} PACKED player_event_t;

/// @brief Function prototype for every player event functions.
///
/// @param server The server structure.
/// @param new_client_idx The player index of the current player.
typedef void (*player_event_handler_t)(server_t PTR server,
    uint32_t player_idx);

/// @brief Function used to handle all events of a client.
///
/// @param server The server structure.
/// @param player_idx The player index we want to handle events.
void handle_player_events(server_t PTR server, uint32_t player_idx);

/// @brief Function used to execute a specific event based on the player_event.
///
/// @param server The server structure.
/// @param player_idx The player index we want to handle events.
/// @param player_event The event to execute.
void execute_player_event_function(server_t PTR server, uint32_t player_idx,
    player_event_t player_event);

/// @brief Events.

/// @brief Function called when a none event is raised.
///
/// @param server The server structure.
/// @param player_idx The player's index.
void execute_player_none_event(__attribute__((unused)) server_t PTR server,
    __attribute__((unused))uint32_t player_idx);

/// @brief Function to create the dead message with the death event.
///
/// @param player The player to destroy.
void create_player_death_event(player_t PTR player);

/// @brief Function called when a death event is raised.
///
/// @param server The server structure.
/// @param player_idx The player's index.
void execute_player_death_event(server_t PTR server, uint32_t player_idx);

/// @brief Function used to verify the requirements for incantation.
///
/// @param server The server structure.
/// @param player_idx The player index.
/// @return True if ok for incantation, False if not.
bool verify_requirements(server_t PTR server, uint16_t player_idx);

/// @brief Function called when a incantation event is raised.
///
/// @param server The server structure.
/// @param player_idx The player's index.
void execute_player_incantation_event(server_t PTR server,
    uint32_t player_idx);
