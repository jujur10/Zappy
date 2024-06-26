/*
** EPITECH PROJECT, 2024
** player_look_command.c
** File description:
** player_look_command.c.
*/
#include "server.h"

#include "resources.h"
#include "utils/arrays/arrays_virtual.h"
#include "utils/arrays/arrays_resources_u32.h"
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
    const player_t PTR player, array_resources_u32_t PTR sorted_resources)
{
    const map_t *map = &server->map;
    const buffer_t *buffer = &server->generated_buffers
        .buffers[PRE_TOP_INDEXES + player->orientation];
    const uint32_t *sorted_indexes = (const uint32_t *)buffer->ptr;
    uint32_t nb_of_tiles_max_depth = get_nb_of_tiles_for_a_depth(MAX_AI_LVL);
    virtual_array_t virtual_array = {};

    set_virtual_array(&virtual_array, &server->map, nb_of_tiles_max_depth,
        player);
    for (uint32_t i = 0; i < (buffer->len / sizeof(uint32_t)) &&
    i < sorted_resources->len; i++) {
        sorted_resources->array[i] =
            map->tiles[get_real_index(&virtual_array, sorted_indexes[i])];
    }
}

static void write_resources_to_buffer(buffer_t PTR buffer,
    const resources_t PTR tile, uint32_t tile_index)
{
    for (uint32_t i = 0; i < tile->arr[tile_index] &&
    i < MAX_RESOURCES_BY_TILE; i++) {
        append_to_buffer_from_chars(buffer, resources_strings[tile_index],
            resources_strings_lengths[tile_index]);
        if (i != tile->arr[tile_index] - 1 && i < MAX_RESOURCES_BY_TILE - 1)
            append_to_buffer_from_chars(buffer, " ", 1);
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

    for (uint32_t tile_index = 0; tile_index < R_STRUCT_SIZE; tile_index++) {
        if (0 == tile->arr[tile_index])
            continue;
        if (true == wrote)
            append_to_buffer_from_chars(buffer, " ", 1);
        write_resources_to_buffer(buffer, tile, tile_index);
        wrote = true;
    }
}

/// @brief Function used to write the vision to the buffer (transform as
/// string).
///
/// @param buffer The buffer to write to.
/// @param sorted_resources The array representing the vision of the player.
/// @param nb_of_tile The number of tiles in sorted_resources.
static void write_vision_to_buffer(buffer_t PTR buffer,
    const array_resources_u32_t PTR sorted_resources)
{
    for (uint32_t i = 0; i < sorted_resources->len; i++) {
        write_tile_content_to_buffer(buffer, &sorted_resources->array[i]);
        if (i != sorted_resources->len - 1)
            append_to_buffer_from_chars(buffer, ",", 1);
    }
}

void execute_player_look_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    array_resources_u32_t sorted_resources;
    buffer_t *buffer = &server->generated_buffers
        .buffers[PRE_LOOK_RESPONSE_BUFFER];
    msg_t message = {};

    ALLOC_RES_ARR_U32_STACK(sorted_resources, nb_of_tile);
    REINITIALIZE_BUFFER(buffer);
    get_sorted_resources(server, player, &sorted_resources);
    append_to_buffer_from_chars(buffer, "[", 1);
    write_vision_to_buffer(buffer, &sorted_resources);
    append_to_buffer_from_chars(buffer, "]\n", 2);
    create_message(buffer->ptr, buffer->len, &message);
    queue_push(&player->queue, &message);
    add_time_limit_to_player(server->time_units, PLAYER_LOOK_WAIT, player);
}
