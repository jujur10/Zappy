/*
** EPITECH PROJECT, 2024
** new_client_event.c
** File description:
** new_client_event.c.
*/
#include <stdint.h>

#include "server.h"
#include "events/new_client_events.h"

/// @brief Function pointer array with every available events for a new client.
static const new_client_event_handler_t event_functions[NEW_CLIENT_NB_EVENT] =
{
    execute_new_client_none_event, execute_new_client_verify_ai_slot_available,
    execute_new_client_verify_guis_slot_available
};

bool execute_new_client_event_function(server_t PTR server,
    uint32_t new_client_idx, msg_t PTR message)
{
    return event_functions[message->event.new_client_event](server,
        new_client_idx, message);
}
