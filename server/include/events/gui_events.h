/*
** EPITECH PROJECT, 2024
** gui_events.h
** File description:
** gui_events.h.
*/

#pragma once

#include "style/macros.h"

/// @brief Redefinition of structures.
typedef struct server_s server_t;
typedef struct player_s player_t;
typedef struct msg_s msg_t;
typedef struct team_s team_t;

/// @brief Enumeration representing the possible gui events to handle.
///
/// @var GUI_EVENT_NONE Representing empty event.
typedef enum {
    GUI_EVENT_NONE,
    GUI_EVENT_UPDATE_MCT,
    GUI_NB_EVENT
} PACKED gui_event_t;

/// @brief Function which sends to GUIs the events of ppo.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
void send_ppo_to_guis(server_t PTR server, const player_t PTR player);

/// @brief Function used to execute a specific event based on the gui_event.
///
/// @param server The server structure.
/// @param gui_idx The gui index we want to handle events.
/// @param gui_event The event to execute.
void execute_gui_event_function(server_t PTR server, uint32_t gui_idx,
    gui_event_t gui_event);

/// @brief Function called when a none event is raised.
///
/// @param server The server structure.
/// @param gui_idx The gui's index.
void execute_gui_none_event(UNUSED server_t PTR server,
    UNUSED uint32_t gui_idx);

/// @brief Function called when a update mct event is raised.
///
/// @param server The server structure.
/// @param gui_idx The gui's index.
void execute_gui_update_mct_event(server_t PTR server,
    UNUSED uint32_t gui_idx);

/// @brief Function used to create a message containing the pnw event.
///
/// @param server The server structure.
/// @param player The player.
/// @param message The message to initialize.
void create_gui_pnw_message(server_t PTR server, const player_t PTR player,
    msg_t PTR message);

/// @brief Function which sends to GUIs the events of pnw.
///
/// @param server The server structure.
/// @param team The player's team.
/// @param player_idx The player index.
void send_pnw_to_guis(server_t PTR server, const team_t PTR team,
    uint32_t player_idx);
