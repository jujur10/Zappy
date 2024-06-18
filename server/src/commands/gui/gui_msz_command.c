/*
** EPITECH PROJECT, 2024
** gui_msz_command.c
** File description:
** gui_msz_command.c.
*/
#include <string.h>

#include "gui_handling.h"
#include "utils/pre_generate/pre_generate.h"

void execute_gui_msz_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command)
{
    char buffer[30] = "msz ";
    msg_t message = {};
    const buffer_t *world_dims_buffer = &server->generated_buffers
        .buffers[PRE_WORLD_DIM_BUFFER];

    memcpy(buffer + 4, world_dims_buffer->ptr, world_dims_buffer->len);
    create_message(buffer, 4 + world_dims_buffer->len, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
