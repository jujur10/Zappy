/*
** EPITECH PROJECT, 2024
** gui_none_command.c
** File description:
** gui_none_command.c.
*/
#include "utils/itoa/fast_itoa.h"
#include "gui_handling.h"

void execute_gui_none_command(server_t PTR server, uint16_t gui_idx,
    __attribute__((unused)) const gui_command_t PTR command)
{
    msg_t message;

    create_message("suc\n", 4, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
