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
    queue_push(&gui->queue, &message);
}
