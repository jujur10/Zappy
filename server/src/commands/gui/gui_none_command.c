/*
** EPITECH PROJECT, 2024
** gui_none_command.c
** File description:
** gui_none_command.c.
*/
#include "utils/itoa/fast_itoa.h"
#include "gui_handling.h"

void execute_gui_none_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command)
{
    gui_suc_response(server, &server->guis[gui_idx]);
}
