/*
** EPITECH PROJECT, 2024
** gui_tna_command.c
** File description:
** gui_tna_command.c.
*/
#include "gui_handling.h"
#include "utils/pre_generate/pre_generate.h"

void execute_gui_tna_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command)
{
    msg_t message = {};
    const buffer_t *tna_buffer = &server->generated_buffers
        .buffers[PRE_TNA_RESPONSE];

    create_message_from_buffer(tna_buffer, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
