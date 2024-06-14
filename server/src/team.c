/*
** EPITECH PROJECT, 2024
** team.c
** File description:
** team.c.
*/
#include <stdint.h>
#include <malloc.h>
#include <stddef.h>
#include <string.h>

#include "arguments.h"
#include "team.h"

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
        (*teams)[i].eggs_coordinates = calloc(args->clients_nb,
        sizeof(*((*teams)->eggs_coordinates)));
    }
    return 0;
}

void destroy_teams(const argument_t PTR args, team_t ARRAY teams)
{
    for (uint32_t i = 0; i < args->nb_of_teams; i++) {
        free(teams[i].players_idx);
        free(teams[i].eggs_coordinates);
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
