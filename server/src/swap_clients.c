/*
** EPITECH PROJECT, 2024
** swap_clients.c
** File description:
** swap_clients.c.
*/
#include <string.h>

#include "style/status.h"
#include "new_clients_handling.h"
#include "gui_handling.h"
#include "ai_handling.h"
#include "utils/itoa/fast_itoa.h"
#include "utils/pre_generate/pre_generate.h"

status_t transform_new_client_to_gui(server_t *server, uint32_t client_idx)
{
    static char buffer[12];
    uint64_t msg_length;
    msg_t message;
    int32_t gui_index;

    msg_length = fast_itoa_u32(MAX_CLIENTS - server->nb_guis, buffer);
    memcpy(buffer + msg_length, "\n\0", 2);
    create_message(buffer, (uint32_t)msg_length + 2, &message);
    gui_index = init_gui(server, server->clients[client_idx].sock);
    if (-1 == gui_index) {
        add_msg_to_queue(&server->clients[client_idx].queue, &message);
        return FAILURE;
    }
    add_msg_to_queue(&server->guis[gui_index].queue, &message);
    create_message(server->generated_buffers
        .buffers[PRE_WORLD_DIM_BUFFER].ptr, server->generated_buffers
        .buffers[PRE_WORLD_DIM_BUFFER].len, &message);
    add_msg_to_queue(&server->guis[gui_index].queue, &message);
    destroy_new_client(server, client_idx, true);
    return SUCCESS;
}

status_t transform_new_client_to_ai(server_t *server, uint32_t client_idx,
    uint32_t team_index)
{
    static char buffer[12];
    uint64_t msg_length;
    msg_t message;
    int32_t ai_index = init_ai(server, server->clients[client_idx].sock);

    msg_length = fast_itoa_u32(server->teams[team_index].max_nb_of_players -
    server->teams[team_index].nb_of_players, buffer);
    memcpy(buffer + msg_length, "\n\0", 2);
    create_message(buffer, (uint32_t)msg_length + 2, &message);
    if (-1 == ai_index) {
        add_msg_to_queue(&server->clients[client_idx].queue, &message);
        return FAILURE;
    }
    add_msg_to_queue(&server->players[ai_index].queue, &message);
    create_message(server->generated_buffers
        .buffers[PRE_WORLD_DIM_BUFFER].ptr, server->generated_buffers
        .buffers[PRE_WORLD_DIM_BUFFER].len, &message);
    add_msg_to_queue(&server->players[ai_index].queue, &message);
    destroy_new_client(server, client_idx, true);
    return SUCCESS;
}
