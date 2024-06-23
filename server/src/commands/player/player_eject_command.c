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
/// @param map The map.
/// @param player_to_eject The player to eject.
/// @param new_coordinates The target coordinates.
/// @param original_direction The original direction of the player.
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

/// @brief Function which verify the egg's coordinates and destroy it if it
/// is on the same tile as the player.
///
/// @param server The server structure.
/// @param team The team containing the egg.
/// @param egg_index The egg index.
/// @param coordinates The coordinates.
static void check_coordinates_and_destroy_egg(server_t PTR server,
    team_t PTR team, uint16_t egg_index,
    const coordinates_t PTR coordinates)
{
    if (true == is_coordinates_equal(coordinates,
    &team->eggs[egg_index].egg_coordinates))
        destroy_egg(server, team, egg_index);
}

/// @brief Function used to destroy every eggs at a given coordinate.
///
/// @param server The server structure.
/// @param coordinates The coordinates we want to destroys every eggs.
static void destroy_eggs_at_coordinates(server_t PTR server,
    const coordinates_t PTR coordinates)
{
    team_t *teams = server->teams;
    const uint16_t nb_of_teams = (uint16_t)server->args->nb_of_teams;

    for (uint16_t team_idx = 0; team_idx < nb_of_teams; team_idx++) {
        for (uint16_t egg_idx = 0; egg_idx < teams[team_idx].nb_of_eggs;
        egg_idx++) {
            check_coordinates_and_destroy_egg(server, &teams[team_idx],
            egg_idx, coordinates);
        }
    }
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
    const coordinates_t *player_pos = &player->coordinates;
    map_t *map = &server->map;
    coordinates_t new_position;

    get_next_player_coordinates(map, player, &new_position);
    destroy_eggs_at_coordinates(server, player_pos);
    if (get_resource_tile_by_coordinates(map, player_pos)->attr.players <= 1)
        return player_ko_response(server, player);
    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (i == player_idx)
            continue;
        if (true == is_coordinates_equal(player_pos, &new_position)) {
            eject_player(map, &server->players[i], &new_position,
                get_direction_of_ejection(player));
            send_pex_to_guis(server, player);
        }
    }
    add_time_limit_to_player(server->time_units, PLAYER_EJECT_WAIT, player);
    return player_ok_response(server, player);
}
