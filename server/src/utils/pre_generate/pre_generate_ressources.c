/*
** EPITECH PROJECT, 2024
** pre_generate_ressources.c
** File description:
** pre_generate_ressources.c.
*/
#include "resources.h"
#include "arguments.h"

resources64_t total_required_resources;

void pre_generate_resources_counter(const argument_t *args)
{
    uint64_t total_cells = args->width * args->height;

    total_required_resources.attr.foods =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_FOODS));
    total_required_resources.attr.linemates =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_LINEMATES));
    total_required_resources.attr.deraumeres =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_DERAUMERES));
    total_required_resources.attr.siburs =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_SIBURS));
    total_required_resources.attr.mendianes =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_MENDIANES));
    total_required_resources.attr.phiras =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_PHIRAS));
    total_required_resources.attr.thystames =
        (uint64_t)((double)total_cells * GET_RES_PROPORTION(NB_OF_THYSTAMES));
    total_required_resources.attr.players = 0;
}
