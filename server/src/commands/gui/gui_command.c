/*
** EPITECH PROJECT, 2024
** gui_command.c
** File description:
** gui_command.c.
*/
#include <string.h>

#include "style/status.h"
#include "server.h"
#include "commands/gui_commands.h"

void (*gui_commands[GUI_NB_OF_CMD]) (server_t *server, uint16_t gui_idx,
    const gui_command_t *command) = {
    execute_gui_none_command, execute_gui_msz_command,
    execute_gui_bct_command, NULL, NULL,
    execute_gui_ppo_command, execute_gui_plv_command,
    execute_gui_pin_command, execute_gui_sgt_command,
    execute_gui_sst_command
};

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
    return gui_commands[command->command](server, gui_idx, command);
}
