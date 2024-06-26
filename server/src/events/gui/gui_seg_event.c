/*
** EPITECH PROJECT, 2024
** gui_seg_event.c
** File description:
** gui_seg_event.c.
*/
#include <stdint.h>

#include "server.h"
#include "gui_handling.h"

void send_seg_to_guis(server_t PTR server, uint32_t winning_team)
{
    string_t string;
    msg_t message = {};

    init_string_from_chars(&string, "seg ", 4);
    append_to_string_from_chars(&string, server->teams[winning_team].name,
        (uint32_t)strlen(server->teams[winning_team].name));
    append_to_string_from_chars(&string, "\n", 1);
    set_event_to_message(&message, (event_t){PLAYER_EVENT_NONE,
        GUI_EVENT_END_OF_GAME, NEW_CLIENT_EVENT_NONE});
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        queue_push(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}

void execute_gui_seg_event(server_t PTR server, uint32_t gui_idx)
{
    destroy_gui(server, gui_idx);
}
