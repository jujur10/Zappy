/*
** EPITECH PROJECT, 2024
** gui_sst_command.c
** File description:
** gui_sst_command.c.
*/
#include "gui_handling.h"
#include "commands/command_utils.h"

void execute_gui_sst_command(server_t PTR server, UNUSED uint16_t gui_idx,
    const gui_command_t PTR command)
{
    char buffer[50] = "sst ";
    uint32_t count = 4;

    server->frequency = command->args[0];
    write_nb_to_buffer(server->frequency, buffer, &count);
    buffer[count - 1] = '\n';
    send_message_to_guis(server, buffer, count);
}
