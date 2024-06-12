/*
** EPITECH PROJECT, 2024
** pre_generate_resources.c
** File description:
** pre_generate_resources.c.
*/
#include "resources.h"
#include "arguments.h"
#include "utils/pre_generate/pre_generate.h"

static void copy_resources_counter_into_buffer(
    const resources64_t *total_required_resources,
    generated_buffers_t *pre_generated_buffers)
{
    init_buffer_from_chars(
        &pre_generated_buffers->buffers[PRE_GENERATED_RESOURCE_COUNTER],
        sizeof(*total_required_resources) + 1,
        (const char *)total_required_resources,
        sizeof(*total_required_resources) + 1
    );
}

void pre_generate_resources_counter(const argument_t *args,
    generated_buffers_t *pre_generated_buffers)
{
    uint64_t total_cells = args->width * args->height;
    resources64_t total_required_resources;

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
    copy_resources_counter_into_buffer(&total_required_resources,
    pre_generated_buffers);
}
