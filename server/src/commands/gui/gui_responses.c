/*
** EPITECH PROJECT, 2024
** gui_responses.c
** File description:
** gui_responses.c.
*/
#include "server.h"

void gui_suc_response(UNUSED const server_t PTR server, gui_t PTR gui)
{
    msg_t message = {};

    create_message("suc\n", 4, &message);
    add_msg_to_queue(&gui->queue, &message);
}

void gui_sbp_response(UNUSED const server_t PTR server, gui_t PTR gui)
{
    msg_t message = {};

    create_message("sbp\n", 4, &message);
    add_msg_to_queue(&gui->queue, &message);
}
