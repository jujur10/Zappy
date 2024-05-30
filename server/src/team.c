/*
** EPITECH PROJECT, 2024
** team.c
** File description:
** team.c.
*/
#include <stdint.h>
#include <malloc.h>
#include <stddef.h>

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