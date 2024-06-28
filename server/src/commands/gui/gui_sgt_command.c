/*
** EPITECH PROJECT, 2024
** gui_sgt_command.c
** File description:
** gui_sgt_command.c.
*/
#include "utils/itoa/fast_itoa.h"
#include "gui_handling.h"
#include "commands/command_utils.h"

void execute_gui_sgt_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command)
{
    char buffer[50] = "sgt ";
    msg_t message = {};
    uint32_t count = 4;

    write_nb_to_buffer(server->frequency, buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, &message);
    queue_push(&server->guis[gui_idx].queue, &message);
}
