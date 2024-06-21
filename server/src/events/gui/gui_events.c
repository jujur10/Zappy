/*
** EPITECH PROJECT, 2024
** gui_events.c
** File description:
** gui_events.c.
*/
#include <stdint.h>

#include "events/gui_events.h"
#include "server.h"

/// @brief Function pointer array with every available events for a GUI.
static void (* const event_functions[GUI_NB_EVENT])(server_t PTR server,
    uint32_t gui_idx) = {
    execute_gui_none_event, execute_gui_update_mct_event
};

void execute_gui_event_function(server_t PTR server, uint32_t gui_idx,
    gui_event_t gui_event)
{
    return event_functions[gui_event](server, gui_idx);
}
