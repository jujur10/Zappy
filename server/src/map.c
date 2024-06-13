/*
** EPITECH PROJECT, 2024
** map.c
** File description:
** map.c.
*/
#include <stdlib.h>

#include "arguments.h"
#include "map.h"
#include "style/status.h"
#include "resources.h"
#include "utils/arrays/arrays_u32.h"
#include "utils/arrays/arrays_u64.h"
#include "utils/pre_generate/pre_generate.h"
#include "game_settings.h"
#include "logging.h"

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

void spread_resources_on_map(map_t PTR map,
    const generated_buffers_t PTR generated_buffers)
{
    int64_t needed_resources[R_STRUCT_SIZE];
    uint32_t x;
    uint32_t y;
    uint32_t index;

    if (false == map->has_been_modified)
        return;
    arrays_sub_u64_to_int64(((const resources64_t *)
    (generated_buffers->buffers[PRE_GENERATED_RESOURCE_COUNTER].ptr))->arr,
    map->total_resources.arr, needed_resources, R_STRUCT_SIZE);
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

void update_map(double current_time, map_t PTR map,
    const generated_buffers_t PTR generated_buffers)
{
    static double next_update = 0;

    next_update = (0 == next_update) ?
        current_time + MAP_UPDATE_WAIT : next_update;
    if (current_time >= next_update) {
        if (true == map->has_been_modified) {
            spread_resources_on_map(map, generated_buffers);
            update_map_buffer(map,
                &generated_buffers->buffers[PRE_MAP_BUFFER]);
        }
        next_update = current_time + MAP_UPDATE_WAIT;
        LOG("Map updated")
    }
}
