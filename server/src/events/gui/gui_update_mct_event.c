/*
** EPITECH PROJECT, 2024
** gui_mct_update_event.c
** File description:
** gui_mct_update_event.c.
*/
#include <stdint.h>

#include "style/macros.h"
#include "server.h"

void execute_gui_update_mct_event(server_t PTR server,
    UNUSED uint32_t gui_idx)
{
    update_map_buffer(&server->map, &server->generated_buffers
    .buffers[PRE_MAP_BUFFER]);
}
