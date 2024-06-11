/*
** EPITECH PROJECT, 2024
** gui_msz_command.c
** File description:
** gui_msz_command.c.
*/
#include <string.h>

#include "utils/pre_generate/pre_generate.h"

void execute_gui_msz_command(gui_t *gui)
{
    char buffer[30] = "msz ";
    msg_t message;

    memcpy(buffer + 4, pre_generated_responses + world_dim_off,
        world_dim_length);
    create_message(buffer, 4 + world_dim_length, &message);
    add_msg_to_queue(&gui->queue, &message);
}
