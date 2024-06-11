/*
** EPITECH PROJECT, 2024
** gui_command.c
** File description:
** gui_command.c.
*/
#include <string.h>

#include "style/status.h"
#include "gui.h"
#include "server.h"

status_t get_next_command(gui_command_buffer_t *gui_command_buffer,
    gui_command_t *next_command)
{
    if (0 == gui_command_buffer->nb_of_command)
        return FAILURE;
    *next_command = gui_command_buffer->commands[0];
    memmove(gui_command_buffer->commands, gui_command_buffer->commands + 1,
        sizeof(gui_command_t) * (gui_command_buffer->nb_of_command - 1));
    gui_command_buffer->nb_of_command--;
    memset(gui_command_buffer->commands + gui_command_buffer->nb_of_command,
        0, sizeof(gui_command_t));
    return SUCCESS;
}

void execute_gui_command(server_t *server, uint16_t gui_idx,
    const gui_command_t *command)
{
    switch (command->command) {
        case GUI_MSZ_CMD:
            return execute_gui_msz_command(&server->guis[gui_idx]);
        case GUI_BCT_CMD:
            return execute_gui_bct_command(server, &server->guis[gui_idx],
            command);
        default:
            return;
    }
}
