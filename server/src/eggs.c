/*
** EPITECH PROJECT, 2024
** eggs.c
** File description:
** eggs.c.
*/
#include "server.h"
#include "style/status.h"

/// @brief Function used to reallocate memory for team's eggs array..
///
/// @param team The team we want to increase capacity.
/// @return Success if reallocation is a success, Failure if not.
static status_t try_reallocate_team_eggs(team_t PTR team)
{
    egg_t *new_egg_array;

    new_egg_array = realloc(team->eggs,
        sizeof(egg_t) * (team->nb_of_allocated_eggs + 5));
    if (NULL == new_egg_array)
        return FAILURE;
    team->eggs = new_egg_array;
    team->nb_of_allocated_eggs += 5;
    return SUCCESS;
}

status_t try_add_egg_to_team(server_t PTR server, team_t PTR team,
    const coordinates_t PTR egg_coordinates)
{
    if (team->nb_of_eggs == team->nb_of_allocated_eggs &&
    FAILURE == try_reallocate_team_eggs(team))
        return FAILURE;
    team->eggs[team->nb_of_eggs].egg_coordinates = *egg_coordinates;
    team->eggs[team->nb_of_eggs].index = server->egg_counter;
    server->egg_counter++;
    team->nb_of_eggs++;
    return SUCCESS;
}
