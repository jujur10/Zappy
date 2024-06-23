/*
** EPITECH PROJECT, 2024
** new_client_events.h
** File description:
** new_client_events.h.
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "style/macros.h"

/// @brief Redefinition of structures.
typedef struct server_s server_t;
typedef struct player_s player_t;
typedef struct msg_s msg_t;
typedef struct team_s team_t;
typedef struct egg_s egg_t;

/// @brief Enumeration representing the possible new client events to handle.
///
/// @var GUI_EVENT_NONE Representing empty event.
/// @var NEW_CLIENT_EVENT_VERIFY_AI_SLOT Representing event of
/// re-verification of AI's available slots (in order to join).
/// @var NEW_CLIENT_EVENT_VERIFY_GUI_SLOT Representing event of
/// re-verification of GUI's available slots (in order to join).
/// @var NEW_CLIENT_NB_EVENT Representing the total number of available events.
typedef enum {
    NEW_CLIENT_EVENT_NONE,
    NEW_CLIENT_EVENT_VERIFY_AI_SLOT,
    NEW_CLIENT_EVENT_VERIFY_GUI_SLOT,
    NEW_CLIENT_NB_EVENT
} PACKED new_client_event_t;

/// @brief Function prototype for every new client event functions.
///
/// @param server The server structure.
/// @param new_client_idx The client index of the new client.
/// @param message The received message (in order to have a context).
/// @return True if the message content should be write, False if not.
typedef bool (*new_client_event_handler_t)(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message);

/// @brief Function used to execute a specific event based on the new client
/// event.
///
/// @param server The server structure.
/// @param new_client_idx The new client index we want to handle events.
/// @param message The received message (in order to have a context).
bool execute_new_client_event_function(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message);

/// @brief Function used to handle if a new slot is available for the new
/// client.
///
/// @param server The server.
/// @param new_client_idx The new client index we want to handle events.
/// @param message The received message (in order to have a context).
bool execute_new_client_verify_ai_slot_available(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message);

/// @brief Function used to handle if a new slot is available for the new
/// client to GUIs.
///
/// @param server The server.
/// @param new_client_idx The new client index we want to handle events.
/// @param message The received message (in order to have a context).
bool execute_new_client_verify_guis_slot_available(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message);

/// @brief Function called when a none event is raised.
///
/// @param server The server structure.
/// @param gui_idx The gui's index.
bool execute_new_client_none_event(UNUSED server_t PTR server,
    UNUSED uint32_t new_client_idx, UNUSED msg_t PTR message);
