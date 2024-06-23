/*
** EPITECH PROJECT, 2024
** new_client_new_slot_available_event.c
** File description:
** new_client_new_slot_available_event.c.
*/
#include "server.h"
#include "clock.h"
#include "events/new_client_events.h"
#include "swap_clients.h"

bool execute_new_client_verify_ai_slot_available(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message)
{
    new_client_t *client = &server->clients[new_client_idx];
    int32_t team_idx = *(int32_t *)(message->ptr);

    if (true == is_timeout_exceed(&server->clock, &client->expiration) &&
    SUCCESS == transform_new_client_to_ai(server, new_client_idx, team_idx)) {
        destroy_message(message);
        return false;
    }
    client->expiration = server->clock;
    add_to_clock(&client->expiration, AUTH_TIMEOUT_SEC, AUTH_TIMEOUT_NS);
    add_msg_to_queue(&server->clients[new_client_idx].queue, message);
    return false;
}

bool execute_new_client_verify_guis_slot_available(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message)
{
    new_client_t *client = &server->clients[new_client_idx];
    int32_t team_idx = *(int32_t *)(message->ptr);

    if (true == is_timeout_exceed(&server->clock, &client->expiration) &&
    SUCCESS == transform_new_client_to_ai(server, new_client_idx, team_idx)) {
        destroy_message(message);
        return false;
    }
    client->expiration = server->clock;
    add_to_clock(&client->expiration, AUTH_TIMEOUT_SEC, AUTH_TIMEOUT_NS);
    add_msg_to_queue(&server->clients[new_client_idx].queue, message);
    return false;
}
