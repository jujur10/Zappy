/*
** EPITECH PROJECT, 2024
** player_eject_command.c
** File description:
** player_eject_command.c.
*/
#include "server.h"
#include "utils/arrays/arrays_virtual.h"
#include "game_settings.h"

/// @brief Function which execute the eject behavior on the selected player.
///
/// \param map The map.
/// \param player_to_eject The player to eject.
/// \param new_coordinates The target coordinates.
/// \param original_direction The original direction of the player.
static void eject_player(map_t PTR map, player_t PTR player_to_eject,
    const coordinates_t PTR new_coordinates,
    orientation_t original_direction)
{
    char msg_content[] = "eject : K\n";
    msg_t message = {};

    teleport_player(map, player_to_eject, new_coordinates);
    msg_content[8] = (char)('0' + original_direction);
    create_message(msg_content, sizeof(msg_content) - 1, &message);
}

/// @brief Function which returns the direction of the ejection.
///
/// @param player The player who eject the others players.
/// @return The direction where the ejection come from.
static orientation_t get_direction_of_ejection(const player_t PTR player)
{
    return ((int32_t)player->orientation - 2
        + NB_OF_ORIENTATION)
        % NB_OF_ORIENTATION;
}

void execute_player_eject_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    const coordinates_t *player_coordinates = &player->coordinates;
    map_t *map = &server->map;
    coordinates_t new_coordinates;
    orientation_t direction_of_ejection = get_direction_of_ejection(player);
    const resources_t *current_tile = &map->tiles
        [(map->width * player_coordinates->y) + player_coordinates->x];

    get_next_player_coordinates(map, player, &new_coordinates);
    if (current_tile->attr.players <= 1)
        return player_ko_response(server, player);
    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (i == player_idx)
            continue;
        if (true == is_coordinates_equal(player_coordinates, &new_coordinates))
            eject_player(map, &server->players[i], &new_coordinates,
                direction_of_ejection);
    }
    add_time_limit_to_player(server->time_units, PLAYER_EJECT_WAIT, player);
    return player_ok_response(server, player);
}
