/*
** EPITECH PROJECT, 2024
** resource.c
** File description:
** resource.c.
*/
#include "resources.h"
#include <string.h>

resources_index_t get_resource_index_from_str(const char *string)
{
    if (0 == strncmp(string, FOOD_STR, sizeof(FOOD_STR)))
        return R_FOOD_IDX;
    if (0 == strncmp(string, LINEMATE_STR, sizeof(LINEMATE_STR)))
        return R_LINEMATE_IDX;
    if (0 == strncmp(string, DERAUMERE_STR, sizeof(DERAUMERE_STR)))
        return R_DERAUMERE_IDX;
    if (0 == strncmp(string, SIBUR_STR, sizeof(SIBUR_STR)))
        return R_SIBUR_IDX;
    if (0 == strncmp(string, MENDIANE_STR, sizeof(MENDIANE_STR)))
        return R_MENDIANE_IDX;
    if (0 == strncmp(string, PHIRAS_STR, sizeof(PHIRAS_STR)))
        return R_PHIRAS_IDX;
    if (0 == strncmp(string, THYSTAME_STR, sizeof(THYSTAME_STR)))
        return R_THYSTAME_IDX;
    return R_STRUCT_SIZE;
}
