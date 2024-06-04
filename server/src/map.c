/*
** EPITECH PROJECT, 2024
** map.c
** File description:
** map.c.
*/
#include <stdint.h>
#include <malloc.h>

#include "arguments.h"
#include "map.h"

uint8_t init_map(const argument_t ARRAY args, map_t ARRAY map)
{
    map->tiles = malloc(sizeof(resources_t) * (args->height * args->width));
    if (NULL == map->tiles)
        return 1;
    map->height = args->height;
    map->width = args->width;
    map->has_been_modified = true;
    return 0;
}
