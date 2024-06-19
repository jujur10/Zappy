/*
** EPITECH PROJECT, 2024
** player_events.c
** File description:
** player_events.c.
*/
#include "server.h"
#include "clock.h"

#include "game_settings.h"

static void (* const event_functions[PLAYER_NB_EVENT])(server_t PTR server,
    uint32_t player_idx) = {
    execute_player_none_event, execute_player_death_event,
    execute_player_incantation_event
};

/// @brief Function used to handle the player health and kill him if he has
/// not enough food to subsist.
///
/// @param server The server.
/// @param player_idx The player index we want to handle events.
static void handle_player_health(server_t PTR server, uint32_t player_idx)
{
    player_t *player = &server->players[player_idx];

    if (true == has_blocking_time_expired(server->time_units,
    player->time_to_eat)) {
        if (0 == player->inventory.attr.food) {
            return create_player_death_event(player);
        }
        player->inventory.attr.food--;
        player->time_to_eat = server->time_units + PLAYER_FOOD_CONSUMTION;
    }
}

void handle_player_events(server_t PTR server, uint32_t player_idx)
{
    handle_player_health(server, player_idx);
}

void execute_event_function(server_t PTR server, uint32_t player_idx,
    player_event_t player_event)
{
    return event_functions[player_event](server, player_idx);
}
