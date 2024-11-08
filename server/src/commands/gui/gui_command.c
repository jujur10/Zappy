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
#include "msg.h"

static void (* const gui_commands[GUI_NB_OF_CMD])(server_t PTR server,
    uint16_t gui_idx, const gui_command_t PTR command) = {
    execute_gui_none_command, execute_gui_msz_command,
    execute_gui_bct_command, execute_gui_mct_command,
    execute_gui_tna_command, execute_gui_ppo_command,
    execute_gui_plv_command, execute_gui_pin_command,
    execute_gui_sgt_command, execute_gui_sst_command
};

status_t get_next_gui_command(gui_command_buffer_t PTR gui_command_buffer,
    gui_command_t PTR next_command)
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

void send_message_to_guis(server_t PTR server, const char PTR ptr,
    uint32_t len)
{
    msg_t message = {};

    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(ptr, len, &message);
        queue_push(&server->guis[i].queue, &message);
    }
}

void send_buffer_to_guis(server_t PTR server, buffer_t PTR buffer,
    gui_event_t gui_event)
{
    msg_t message = {};

    create_message_from_buffer(buffer, &message);
    message.event.gui_event = gui_event;
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        queue_push(&server->guis[i].queue, &message);
    }
}

void execute_gui_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    return gui_commands[command->command](server, gui_idx, command);
}
