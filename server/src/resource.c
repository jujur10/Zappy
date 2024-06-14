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
    if (0 == strncmp(string, R_FOOD_STR, sizeof(R_FOOD_STR)))
        return R_FOOD_IDX;
    if (0 == strncmp(string, R_LINEMATE_STR, sizeof(R_LINEMATE_STR)))
        return R_LINEMATE_IDX;
    if (0 == strncmp(string, R_DERAUMERE_STR, sizeof(R_DERAUMERE_STR)))
        return R_DERAUMERE_IDX;
    if (0 == strncmp(string, R_SIBUR_STR, sizeof(R_SIBUR_STR)))
        return R_SIBUR_IDX;
    if (0 == strncmp(string, R_MENDIANE_STR, sizeof(R_MENDIANE_STR)))
        return R_MENDIANE_IDX;
    if (0 == strncmp(string, R_PHIRAS_STR, sizeof(R_PHIRAS_STR)))
        return R_PHIRAS_IDX;
    if (0 == strncmp(string, R_THYSTAME_STR, sizeof(R_THYSTAME_STR)))
        return R_THYSTAME_IDX;
    return R_STRUCT_SIZE;
}
