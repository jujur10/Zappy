/*
** EPITECH PROJECT, 2024
** gui_events.c
** File description:
** gui_events.c.
*/
#include <stdint.h>

#include "events/gui_events.h"
#include "server.h"

/// @brief Function pointer array with every available pre write events for a
/// GUI.
static const gui_event_handler_t pre_event_functions[GUI_NB_EVENT] = {
    execute_gui_none_event, execute_gui_update_mct_event,
    execute_gui_none_event
};

/// @brief Function pointer array with every available post write events for a
/// GUI.
static const gui_event_handler_t post_event_functions[GUI_NB_EVENT] = {
    execute_gui_none_event, execute_gui_none_event, execute_gui_seg_event
};

void execute_gui_pre_event_function(server_t PTR server, uint32_t gui_idx,
    gui_event_t gui_event)
{
    return pre_event_functions[gui_event](server, gui_idx);
}

void execute_gui_post_event_function(server_t PTR server, uint32_t gui_idx,
    gui_event_t gui_event)
{
    return post_event_functions[gui_event](server, gui_idx);
}
