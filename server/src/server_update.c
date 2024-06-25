/*
** EPITECH PROJECT, 2024
** server_update.c
** File description:
** server_update.c.
*/
#include "server.h"
#include "clock.h"
#include "server_update.h"

/// @brief Function which update the in game time. (based on real time clock
/// and frequency parameter).
///
/// @param server The server to update.
static void update_game_clock(server_t PTR server)
{
    static bool first_time = true;
    static struct timespec old_clock = {};

    clock_gettime(CLOCK_MONOTONIC, &server->clock);
    old_clock = (true == first_time) ? server->clock : old_clock;
    server->time_units += real_time_clock_to_time_units(server->frequency,
    &server->clock, &old_clock);
    old_clock = server->clock;
    first_time = false;
}

void update_server(server_t PTR server)
{
    update_game_clock(server);
    check_end_of_the_game(server);
    if (true == update_map(server->time_units, &server->map,
    &server->generated_buffers))
        send_buffer_to_guis(server, &server->generated_buffers
        .buffers[PRE_MAP_BUFFER], GUI_EVENT_UPDATE_MCT);
}
