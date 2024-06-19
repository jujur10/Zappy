/*
** EPITECH PROJECT, 2024
** team.c
** File description:
** team.c.
*/
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "team.h"

/// @brief Function which set random coordinates to eggs.
///
/// @param team The team we want to set eggs coordinates to.
/// @param map_width The map width.
/// @param map_height The map height.
static void set_random_coordinates_to_eggs(team_t PTR team,
    uint32_t map_width, uint32_t map_height)
{
    for (uint16_t i = 0; i < team->nb_of_eggs; i++) {
        team->eggs[i].index = team->egg_counter;
        team->eggs[i].egg_coordinates.x = (uint16_t)rand() % map_width;
        team->eggs[i].egg_coordinates.y = (uint16_t)rand() % map_height;
        team->egg_counter++;
    }
}

uint8_t init_teams(const argument_t PTR args, team_t PTR ARRAY teams)
{
    *teams = calloc(args->nb_of_teams, sizeof(team_t));
    if (NULL == *teams)
        return 1;
    for (uint32_t i = 0; i < args->nb_of_teams; i++) {
        (*teams)[i].name = args->team_names[i];
        (*teams)[i].max_nb_of_players = args->clients_nb;
        (*teams)[i].players_idx = calloc(args->clients_nb,
        sizeof(*((*teams)->players_idx)));
        (*teams)[i].nb_of_eggs = args->clients_nb;
        (*teams)[i].nb_of_allocated_eggs = args->clients_nb;
        (*teams)[i].eggs = calloc(args->clients_nb,
        sizeof(*((*teams)->eggs)));
        set_random_coordinates_to_eggs(&(*teams)[i], args->width,
            args->height);
        (*teams)[i].egg_counter = 0;
    }
    return 0;
}

void destroy_teams(const argument_t PTR args, team_t ARRAY teams)
{
    for (uint32_t i = 0; i < args->nb_of_teams; i++) {
        free(teams[i].players_idx);
        free(teams[i].eggs);
    }
    free(teams);
}

int32_t get_team_index_by_name(const team_t ARRAY teams, uint32_t nb_of_teams,
    const char ARRAY searched_name, uint32_t len)
{
    for (uint32_t i = 0; i < nb_of_teams; i++)
        if (0 == strncmp(teams[i].name, searched_name, len))
            return (int32_t)i;
    return -1;
}

uint16_t get_nb_of_unused_slot(const team_t PTR team)
{
    int32_t theoretical = team->max_nb_of_players - team->nb_of_players;

    theoretical = (theoretical < 0) ? 0 : theoretical;
    return (theoretical < (int32_t)team->nb_of_eggs) ? (uint16_t)theoretical :
        team->nb_of_eggs;
}
