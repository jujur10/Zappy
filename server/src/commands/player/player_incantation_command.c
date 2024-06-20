/*
** EPITECH PROJECT, 2024
** player_incantation_command.c
** File description:
** player_incantation_command.c.
*/
#include "server.h"
#include "resources.h"
#include "game_settings.h"
#include "commands/player_commands.h"
#include "commands/command_utils.h"
#include "utils/itoa/fast_itoa.h"

/// @brief Array of resources_t representing the needed elements for elevation.
const resources_t elevation_requirements[] = {
    {{0, 1, 0, 0, 0, 0, 0, 1}},
    {{0, 1, 1, 1, 0, 0, 0, 2}},
    {{0, 2, 0, 1, 0, 2, 0, 2}},
    {{0, 1, 1, 2, 0, 1, 0, 4}},
    {{0, 1, 2, 1, 3, 0, 0, 4}},
    {{0, 1, 2, 3, 0, 1, 0, 6}},
    {{0, 2, 2, 2, 2, 2, 1, 6}},
};

/// @brief Function which verifies if the tile have the minimum requirements
/// to proceed to the elevation.
///
/// @param tile The tile where the elevation is produced.
/// @param target_level The target level.
/// @return True if the requirements are ok, False if not.
static bool verify_tile_requirements(const resources_t *tile,
    elevation_requirements_t target_level)
{
    for (uint32_t i = 0; i < R_STRUCT_SIZE; i++)
        if (tile->arr[i] < elevation_requirements[target_level].arr[i])
            return false;
    return true;
}

/// @brief Function which verifies if the number of players is correct, and
/// if they are of the same level as the original player.
///
/// @param players The players array.
/// @param nb_of_players The number of players into the array.
/// @param player_idx The player's index of the player at the origin of the
/// incantation.
/// @return The number of players ready.
static uint16_t get_nb_of_players_ready(const player_t ARRAY players,
    uint16_t nb_of_players, uint16_t player_idx)
{
    const player_t *player = &players[player_idx];
    const player_t *other_player;
    uint16_t player_counter = 0;

    for (uint16_t i = 0; i < nb_of_players; i++) {
        other_player = &players[i];
        if (false == is_coordinates_equal(&player->coordinates,
            &other_player->coordinates))
            continue;
        if (player->level != other_player->level)
            continue;
        player_counter++;
    }
    return player_counter;
}

/// @brief Function which verifies the players requirements.
///
/// @param players The players array.
/// @param nb_of_players The number of players into the array.
/// @param player_idx The player's index of the player at the origin of the
/// incantation.
/// @return True if the player requirements are ok, False if not.
static bool verify_player_requirements(const player_t ARRAY players,
    uint16_t nb_of_players, uint16_t player_idx)
{
    const player_t *player = &players[player_idx];
    uint16_t nb_of_players_ready = get_nb_of_players_ready(players,
        nb_of_players, player_idx);

    if (nb_of_players_ready < elevation_requirements[player->level]
    .attr.players)
        return false;
    return true;
}

/// @brief Function which verify the requirements for the elevations.
/// Verify :
/// - Requirements for the tile.
/// - Requirements for the players.
///
/// @param server The server.
/// @param player_idx The player index of the player at the origin of
/// incantation.
/// @return True for success, False for failure.
bool verify_requirements(server_t PTR server, uint16_t player_idx)
{
    const player_t *player = &server->players[player_idx];
    const resources_t *current_tile = &server->map.tiles[(player->coordinates
        .y * server->map.width) + player->coordinates.x];

    if (true == verify_tile_requirements(current_tile, player->level) &&
        true == verify_player_requirements(server->players, server->nb_players,
        player_idx))
        return true;
    return false;
}

/// @brief Function used to write the player indexes (sockets) into the string.
///
/// @param server The server structure.
/// @param player_idx The player index.
/// @param string The string to write to.
static void write_ready_players_into_string(server_t PTR server,
    uint16_t player_idx, string_t PTR string)
{
    const player_t *player = &server->players[player_idx];
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1] = "pic ";
    uint32_t count;

    for (uint32_t i = 0; i < server->nb_players; i++) {
        if (i != player_idx && true == is_coordinates_equal
        (&player->coordinates, &server->players[i].coordinates) &&
        player->level == server->players[i].level) {
            count = (uint32_t)fast_itoa_u32(server->players[i].sock,
                msg_content);
            msg_content[count] = ' ';
            append_to_string_from_chars(string, msg_content, count + 1);
        }
    }
    string->ptr[string->len - 1] = '\n';
}

/// @brief Function which sends to GUIs the events of pic.
///
/// @param server The server structure.
/// @param team The player's team.
/// @param player_idx The player index.
static void send_pic_to_guis(server_t PTR server, uint32_t player_idx)
{
    msg_t message;
    char msg_content[4 + (3 * UINT32_MAX_DIGITS) + 1] = "pic ";
    uint32_t count = 4;
    const player_t *player = &server->players[player_idx];
    string_t string;

    write_nb_to_buffer(player->coordinates.x, msg_content, &count);
    write_nb_to_buffer(player->coordinates.y, msg_content, &count);
    write_nb_to_buffer(player->level + 1, msg_content, &count);
    init_string_from_chars(&string, msg_content, count);
    write_ready_players_into_string(server, (uint16_t)player_idx, &string);
    for (uint16_t i = 0; i < server->nb_guis; i++) {
        create_message(string.ptr, string.len, &message);
        add_msg_to_queue(&server->guis[i].queue, &message);
    }
    clear_string(&string);
}

void execute_player_incantation_command(server_t PTR server,
    uint16_t player_idx, UNUSED const player_command_t PTR command)
{
    msg_t message = {};
    player_t *player = &server->players[player_idx];

    if (true == verify_requirements(server, player_idx)) {
        send_pic_to_guis(server, player_idx);
        create_message("Elevation underway\n", 19, &message);
        message.event.player_event = PLAYER_EVENT_INCANTATION;
        add_msg_to_queue(&player->queue, &message);
        return add_time_limit_to_player(server->time_units,
            PLAYER_INCANTATION_WAIT, player);
    }
    return player_ko_response(server, player);
}
