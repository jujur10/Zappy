/*
** EPITECH PROJECT, 2024
** map.c
** File description:
** map.c.
*/
#include <stdlib.h>

#include "arguments.h"
#include "map.h"
#include "commands/gui_commands.h"
#include "style/status.h"
#include "resources.h"
#include "utils/arrays/arrays_u32.h"
#include "utils/arrays/arrays_u64.h"
#include "utils/pre_generate/pre_generate.h"
#include "game_settings.h"
#include "logging.h"
#include "coordinates.h"

status_t init_map(const argument_t ARRAY args, map_t PTR map)
{
    map->tiles = calloc(args->height * args->width, sizeof(resources_t));
    if (NULL == map->tiles)
        return FAILURE;
    map->height = (uint16_t)args->height;
    map->width = (uint16_t)args->width;
    map->has_been_modified = true;
    return SUCCESS;
}

/// @brief Function used to calculate the total number of resources on the map.
///
/// @param map The map.
/// @param result The result variable.
static void get_map_total_resources(const map_t PTR map,
    resources64_t PTR result)
{
    const resources_t *tiles = map->tiles;

    for (uint32_t i = 0; i < map->width * map->height; i++)
        for (uint32_t resource_idx = 0; resource_idx < R_STRUCT_SIZE - 1;
        resource_idx++)
            result->arr[resource_idx] += tiles[i].arr[resource_idx];
}

void spread_resources_on_map(map_t PTR map,
    const generated_buffers_t PTR generated_buffers)
{
    int64_t needed_resources[R_STRUCT_SIZE];
    resources64_t total_resources = {};
    uint32_t x;
    uint32_t y;
    uint32_t index;

    get_map_total_resources(map, &total_resources);
    arrays_sub_u64_to_int64(((const resources64_t *)
    (generated_buffers->buffers[PRE_GENERATED_RESOURCE_COUNTER].ptr))->arr,
    total_resources.arr, needed_resources, R_STRUCT_SIZE);
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++) {
        for (uint32_t j = 0; j < needed_resources[i]; j++) {
            x = rand() % map->width;
            y = rand() % map->height;
            index = (y * map->width) + x;
            map->tiles[index].arr[i] += (needed_resources[i] > 0) ? 1 : 0;
            map->total_resources.arr[i] += (needed_resources[i] > 0) ? 1 : 0;
        }
    }
    map->has_been_modified = false;
}

bool update_map(double current_time, map_t PTR map,
    const generated_buffers_t PTR generated_buffers)
{
    static double next_update = 0;

    next_update = (0 == next_update) ?
        current_time + MAP_UPDATE_WAIT : next_update;
    if (current_time >= next_update) {
        spread_resources_on_map(map, generated_buffers);
        next_update = current_time + MAP_UPDATE_WAIT;
        LOG("Map updated")
        return true;
    }
    return false;
}

resources_t *get_resource_tile_by_coordinates(const map_t PTR map,
    const coordinates_t PTR coordinates)
{
    return &map->tiles[(coordinates->y * map->width) + coordinates->x];
}
