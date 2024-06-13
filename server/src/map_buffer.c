/*
** EPITECH PROJECT, 2024
** map_buffer.c
** File description:
** map_buffer.c.
*/
#include "server.h"

#include "utils/string/buffer.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

void add_tile_to_buffer(uint32_t x, uint32_t y, buffer_t PTR map_buff,
    const resources_t PTR tile)
{
    static char temp_buffer[(NB_OF_MAP_AXIS + R_STRUCT_SIZE - 1) *
        (UINT32_MAX_DIGITS + 1) + BCT_COMMAND_LEN] = "bct ";
    uint32_t wrote = 4;

    write_nb_to_buffer(x, temp_buffer, &wrote);
    write_nb_to_buffer(y, temp_buffer, &wrote);
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++)
        write_nb_to_buffer(tile->arr[i], temp_buffer, &wrote);
    temp_buffer[wrote - 1] = '\n';
    append_to_buffer_from_chars(map_buff, temp_buffer, wrote);
}

void update_map_buffer(const map_t PTR map, buffer_t PTR buffer)
{
    const resources_t *tiles = map->tiles;

    REINITIALIZE_BUFFER(buffer);
    for (uint32_t y = 0; y < map->height; y++)
        for (uint32_t x = 0; x < map->width; x++)
            add_tile_to_buffer(x, y, buffer, &tiles[(y * map->width) + x]);
}
