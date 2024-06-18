/*
** EPITECH PROJECT, 2024
** player_look_command.c
** File description:
** player_look_command.c.
*/
#include "server.h"

#include "resources.h"
#include "utils/arrays/arrays_virtual.h"
#include "game_settings.h"

/// @brief Array representing the resources strings in order to be able to
// iterate over it.
static const char *const resources_strings[] = {
    FOOD_STR, LINEMATE_STR, DERAUMERE_STR, SIBUR_STR, MENDIANE_STR,
    PHIRAS_STR, THYSTAME_STR, PLAYER_STR
};

/// @brief Array representing the resources strings lengths in order to be
/// able to iterate over it.
static const uint32_t resources_strings_lengths[] = {
    sizeof(FOOD_STR) - 1, sizeof(LINEMATE_STR) - 1, sizeof(DERAUMERE_STR) - 1,
    sizeof(SIBUR_STR) - 1, sizeof(MENDIANE_STR) - 1, sizeof(PHIRAS_STR) - 1,
    sizeof(THYSTAME_STR) - 1, sizeof(PLAYER_STR) - 1
};

/// @brief Function used to obtain the sorted array of tile when the player
/// is looking to the "top" direction.
///
/// @param server The server structure.
/// @param virtual_array The virtual array representing the raw vision of
/// the player.
/// @param sorted_resources The output array to fill.
/// @param nb_of_tile The output array's length.
static void get_sorted_resources_top(const server_t PTR server,
    virtual_array_t PTR virtual_array,
    resources_t PTR sorted_resources, uint32_t nb_of_tile)
{
    const map_t *map = &server->map;
    const buffer_t *top_buffer = &server->generated_buffers
        .buffers[PRE_TOP_INDEXES];
    const uint32_t *sorted_indexes = (const uint32_t *)top_buffer->ptr;

    for (uint32_t i = 0; i < (top_buffer->len / sizeof(uint32_t))
        && i < nb_of_tile; i++)
        sorted_resources[i] = map->tiles[get_real_index(virtual_array,
            sorted_indexes[i])];
}

/// @brief Function used to obtain the sorted array of tile when the player
/// is looking to the "bottom" direction.
///
/// @param server The server structure.
/// @param virtual_array The virtual array representing the raw vision of
/// the player.
/// @param sorted_resources The output array to fill.
/// @param nb_of_tile The output array's length.
static void get_sorted_resources_bottom(const server_t PTR server,
    virtual_array_t PTR virtual_array,
    resources_t PTR sorted_resources, uint32_t nb_of_tile)
{
    const map_t *map = &server->map;
    const buffer_t *bottom_buffer = &server->generated_buffers
        .buffers[PRE_BOTTOM_INDEXES];
    const uint32_t *sorted_indexes = (const uint32_t *)bottom_buffer->ptr;

    for (uint32_t i = 0; i < (bottom_buffer->len / sizeof(uint32_t))
        && i < nb_of_tile; i++)
        sorted_resources[i] = map->tiles[get_real_index(virtual_array,
            sorted_indexes[i])];
}

/// @brief Function used to obtain the sorted array of tile when the player
/// is looking to the "right" direction.
///
/// @param server The server structure.
/// @param virtual_array The virtual array representing the raw vision of
/// the player.
/// @param sorted_resources The output array to fill.
/// @param nb_of_tile The output array's length.
static void get_sorted_resources_right(const server_t PTR server,
    virtual_array_t PTR virtual_array,
    resources_t PTR sorted_resources, uint32_t nb_of_tile)
{
    const map_t *map = &server->map;
    const buffer_t *right_buffer = &server->generated_buffers
        .buffers[PRE_RIGHT_INDEXES];
    const uint32_t *sorted_indexes = (const uint32_t *)right_buffer->ptr;

    for (uint32_t i = 0; i < (right_buffer->len / sizeof(uint32_t))
        && i < nb_of_tile; i++)
        sorted_resources[i] = map->tiles[get_real_index(virtual_array,
            sorted_indexes[i])];
}

/// @brief Function used to obtain the sorted array of tile when the player
/// is looking to the "left" direction.
///
/// @param server The server structure.
/// @param virtual_array The virtual array representing the raw vision of
/// the player.
/// @param sorted_resources The output array to fill.
/// @param nb_of_tile The output array's length.
static void get_sorted_resources_left(const server_t PTR server,
    virtual_array_t PTR virtual_array,
    resources_t PTR sorted_resources, uint32_t nb_of_tile)
{
    const map_t *map = &server->map;
    const buffer_t *left_buffer = &server->generated_buffers
        .buffers[PRE_LEFT_INDEXES];
    const uint32_t *sorted_indexes = (const uint32_t *)left_buffer->ptr;

    for (uint32_t i = 0; i < (left_buffer->len / sizeof(uint32_t))
        && i < nb_of_tile; i++)
        sorted_resources[i] = map->tiles[get_real_index(virtual_array,
            sorted_indexes[i])];
}

/// @brief Function used to setup the virtual arrays offset in order to
/// represent the raw view of the player.
///
/// @param virtual_array The array we want to sets the offsets.
/// @param player_orientation The current player orientation.
/// @param coordinates The coordinates of the player.
static void set_virtual_array_offset(virtual_array_t PTR virtual_array,
    orientation_t player_orientation, const coordinates_t PTR coordinates)
{
    if (LOOK_TOP == player_orientation)
        return set_offsets(virtual_array,
            (int32_t)(coordinates->x - (virtual_array->virtual_width / 2)),
            (int32_t)(coordinates->y - virtual_array->virtual_height + 1)
        );
    if (LOOK_BOTTOM == player_orientation)
        return set_offsets(virtual_array,
            (int32_t)(coordinates->x - (virtual_array->virtual_width / 2)),
            (int32_t)(coordinates->y)
        );
    if (LOOK_RIGHT == player_orientation)
        return set_offsets(virtual_array,
            (int32_t)(coordinates->x),
            (int32_t)(coordinates->y - (virtual_array->virtual_height / 2))
        );
    if (LOOK_LEFT == player_orientation)
        return set_offsets(virtual_array,
            (int32_t)(coordinates->x - virtual_array->virtual_width + 1),
            (int32_t)(coordinates->y - (virtual_array->virtual_height / 2))
        );
}

/// @brief Function used to setup the virtual array.
///
/// @param virtual_array The virtual array we wants to set up.
/// @param map The map.
/// @param nb_of_tiles_max_depth The number of tiles at the maximal depth.
/// @param player The player structure representing the actual client.
static void set_virtual_array(virtual_array_t PTR virtual_array,
    const map_t PTR map, uint32_t nb_of_tiles_max_depth,
    const player_t PTR player)
{
    virtual_array->original_width = map->width;
    virtual_array->original_height = map->height;
    virtual_array->virtual_width = (LOOK_TOP == player->orientation ||
        LOOK_BOTTOM == player->orientation) ? nb_of_tiles_max_depth :
        MAX_AI_LVL + 1;
    virtual_array->virtual_height = (LOOK_TOP == player->orientation ||
        LOOK_BOTTOM == player->orientation) ? MAX_AI_LVL + 1 :
        nb_of_tiles_max_depth;
    set_virtual_array_offset(virtual_array, player->orientation,
        &player->coordinates);
}

void get_sorted_resources(const server_t PTR server,
    player_t PTR player,
    resources_t ARRAY sorted_resources, uint32_t nb_of_tile)
{
    uint32_t nb_of_tiles_max_depth = get_nb_of_tiles_for_a_depth(MAX_AI_LVL);
    virtual_array_t virt_arr;

    set_virtual_array(&virt_arr, &server->map, nb_of_tiles_max_depth, player);
    switch (player->orientation) {
        case LOOK_TOP:
            return get_sorted_resources_top(server, &virt_arr,
                sorted_resources, nb_of_tile);
        case LOOK_BOTTOM:
            return get_sorted_resources_bottom(server, &virt_arr,
                sorted_resources, nb_of_tile);
        case LOOK_RIGHT:
            return get_sorted_resources_right(server, &virt_arr,
                sorted_resources, nb_of_tile);
        case LOOK_LEFT:
            return get_sorted_resources_left(server, &virt_arr,
                sorted_resources, nb_of_tile);
        default:
            return player_ko_response(server, player);
    }
}

/// @brief Function which write the tile content into the buffer.
///
/// @param buffer The buffer we want to write to.
/// @param tile The tile we want to transform in string.
static void write_tile_content_to_buffer(buffer_t PTR buffer,
    const resources_t PTR tile)
{
    bool wrote = false;

    for (uint32_t i = 0; i < R_STRUCT_SIZE; i++) {
        if (0 == tile->arr[i])
            continue;
        if (true == wrote)
            append_to_buffer_from_chars(buffer, " ", 1);
        append_to_buffer_from_chars(buffer, resources_strings[i],
            resources_strings_lengths[i]);
        wrote = true;
    }
}

void execute_player_look_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile] = {};
    buffer_t *buffer = &server->generated_buffers
        .buffers[PRE_LOOK_RESPONSE_BUFFER];
    msg_t message = {};

    REINITIALIZE_BUFFER(buffer);
    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    append_to_buffer_from_chars(buffer, "[", 1);
    for (uint32_t i = 0; i < nb_of_tile; i++) {
        write_tile_content_to_buffer(buffer, &sorted_resources[i]);
        if (i != nb_of_tile - 1)
            append_to_buffer_from_chars(buffer, ",", 1);
    }
    append_to_buffer_from_chars(buffer, "]", 1);
    create_message(buffer->ptr, buffer->len, &message);
    add_msg_to_queue(&player->queue, &message);
}
