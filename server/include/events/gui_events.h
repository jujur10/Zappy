/*
** EPITECH PROJECT, 2024
** gui_events.h
** File description:
** gui_events.h.
*/

#pragma once

#include "style/macros.h"
#include "utils/string/buffer.h"
#include "resources.h"

/// @brief Redefinition of structures.
typedef struct server_s server_t;
typedef struct player_s player_t;
typedef struct msg_s msg_t;
typedef struct team_s team_t;
typedef struct egg_s egg_t;

/// @brief Enumeration representing the possible gui events to handle.
///
/// @var GUI_EVENT_NONE Representing empty event.
/// @var GUI_EVENT_UPDATE_MCT Representing that the mct buffer should be
/// updated before sending the message.
/// @var PLAYER_EVENT_END_OF_GAME Representing the end of the game event.
/// @var GUI_NB_EVENT Representing the total number of events.
typedef enum {
    GUI_EVENT_NONE,
    GUI_EVENT_UPDATE_MCT,
    GUI_EVENT_END_OF_GAME,
    GUI_NB_EVENT
} PACKED gui_event_t;

/// @brief Function prototype for every player event functions.
///
/// @param server The server structure.
/// @param gui_idx The gui index of the gui.
typedef void (*gui_event_handler_t)(server_t PTR server, uint32_t gui_idx);

/// @brief Function which sends to GUIs the events of ppo.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
void send_ppo_to_guis(server_t PTR server, const player_t PTR player);

/// @brief Function used to execute a specific event based on the gui_event.
/// (before write).
///
/// @param server The server structure.
/// @param gui_idx The gui index we want to handle events.
/// @param gui_event The event to execute.
void execute_gui_pre_event_function(server_t PTR server, uint32_t gui_idx,
    gui_event_t gui_event);

/// @brief Function used to execute a specific event based on the gui_event
/// (after write).
///
/// @param server The server structure.
/// @param gui_idx The gui index we want to handle events.
/// @param gui_event The event to execute.
void execute_gui_post_event_function(server_t PTR server, uint32_t gui_idx,
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

/// @brief Function which sends to GUIs the events of pbc.
///
/// @param server The server structure.
/// @param message_buffer The message buffer.
/// @param player_idx The player index.
void send_pbc_to_guis(server_t PTR server, const buffer_t PTR message_buffer,
    uint32_t player_idx);

/// @brief Function which sends to GUIs the events of pex.
///
/// @param server The server structure.
/// @param egg_index The egg index of the taken egg.
void send_pex_to_guis(server_t PTR server, const player_t PTR player);

/// @brief Function which sends to GUIs the events of enw.
///
/// @param server The server structure.
/// @param egg The egg.
/// @param player The player involved.
void send_enw_to_guis(server_t PTR server, const egg_t PTR egg,
    const player_t PTR player);

/// @brief Function which sends to GUIs the events of pfk.
///
/// @param server The server structure.
/// @param player The player involved.
void send_pfk_to_guis(server_t PTR server, const player_t PTR player);

/// @brief Function which sends to GUIs the events of pic.
///
/// @param server The server structure.
/// @param player_idx The player index.
void send_pic_to_guis(server_t PTR server, uint32_t player_idx);

/// @brief Function which sends to GUIs the events of pdr.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
void send_pdr_to_guis(server_t PTR server, const player_t PTR player,
    resources_index_t resource_index);

/// @brief Function which sends to GUIs the events of pgt.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
void send_pgt_to_guis(server_t PTR server, const player_t PTR player,
    resources_index_t resource_index);

/// @brief Function which sends to GUIs the events of seg.
///
/// @param server The server structure.
/// @param winning_team The winning team.
void send_seg_to_guis(server_t PTR server, uint32_t winning_team);

/// @brief Function which sends to GUIs the events of pdi.
///
/// @param server The server structure.
/// @param player_idx The player index.
void send_pdi_to_guis(server_t PTR server, uint32_t player_idx);

/// @brief Function which executes the seg event (end of the game).
/// - Destroys the GUI.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
void execute_gui_seg_event(server_t PTR server, uint32_t gui_idx);
