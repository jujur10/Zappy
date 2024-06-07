/*
** EPITECH PROJECT, 2024
** map.c
** File description:
** map.c.
*/
#include <malloc.h>
#include <stdlib.h>

#include "arguments.h"
#include "map.h"
#include "style/status.h"
#include "resources.h"
#include "utils/arrays/arrays_u32.h"
#include "utils/arrays/arrays_u64.h"
#include "utils/pre_generate/pre_generate.h"

status_t init_map(const argument_t ARRAY args, map_t ARRAY map)
{
    map->tiles = malloc(sizeof(resources_t) * (args->height * args->width));
    if (NULL == map->tiles)
        return FAILURE;
    map->height = args->height;
    map->width = args->width;
    map->has_been_modified = true;
    return SUCCESS;
}

void spread_resources_on_map(map_t *map)
{
    int64_t needed_resources[R_STRUCT_SIZE];
    uint32_t x;
    uint32_t y;
    uint32_t index;

    if (false == map->has_been_modified)
        return;
    arrays_sub_u64_to_int64(total_required_resources.arr,
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
