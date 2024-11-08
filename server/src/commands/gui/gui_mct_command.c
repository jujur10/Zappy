/*
** EPITECH PROJECT, 2024
** gui_mct_command.c
** File description:
** gui_mct_command.c.
*/
#include "gui_handling.h"
#include "utils/pre_generate/pre_generate.h"
#include "utils/itoa/fast_itoa.h"

void execute_gui_mct_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command)
{
    msg_t message = {};

    create_message_from_buffer(&server->generated_buffers
    .buffers[PRE_MAP_BUFFER], &message);
    set_event_to_message(&message,
        (event_t){PLAYER_EVENT_NONE, GUI_EVENT_UPDATE_MCT,
        NEW_CLIENT_EVENT_NONE});
    queue_push(&server->guis[gui_idx].queue, &message);
}
