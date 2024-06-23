/*
** EPITECH PROJECT, 2024
** pre_generate_look_indexes.c
** File description:
** pre_generate_look_indexes.c.
*/
#include "utils/pre_generate/pre_generate.h"
#include "game_settings.h"

uint32_t get_nb_of_tiles_for_a_depth(uint32_t depth)
{
    return (depth * 2) + 1;
}

uint32_t get_total_nb_of_tiles_required(uint8_t level)
{
    return ((get_nb_of_tiles_for_a_depth(0) + get_nb_of_tiles_for_a_depth
        (level)) * (level + 1)) / 2;
}

/// @brief Function which calculates all the indexes of bottom view sorted as
/// would.
///
/// @param buffer The buffer to fill.
/// @param nb_of_tiles_max_depth The maximal number of tile for a depth.
/// @param depth The depth.
static void get_bottom_indexes(buffer_t PTR buffer,
    uint32_t nb_of_tiles_max_depth, uint32_t depth)
{
    uint32_t n;

    for (uint32_t i = 0; i < depth; i++) {
        n = (nb_of_tiles_max_depth / 2) + i;
        for (int32_t j = (int32_t)n; j > (int32_t)(n -
        get_nb_of_tiles_for_a_depth(i)); j--) {
            append_to_buffer_from_chars(buffer,
                (char *)&(uint32_t){(i * nb_of_tiles_max_depth) + j},
                sizeof(uint32_t)
            );
        }
    }
}

/// @brief Function which calculates all the indexes of top view sorted as
/// would.
///
/// @param buffer The buffer to fill.
/// @param nb_of_tiles_max_depth The maximal number of tile for a depth.
/// @param depth The depth.
static void get_top_indexes(buffer_t PTR buffer,
    uint32_t nb_of_tiles_max_depth, uint32_t depth)
{
    uint32_t n;

    for (uint32_t i = 0; i < depth; i++) {
        n = (nb_of_tiles_max_depth / 2) - i;
        for (uint32_t j = n; j < n + get_nb_of_tiles_for_a_depth(i); j++) {
            append_to_buffer_from_chars(buffer,
                (char *)&(uint32_t){((depth - 1 - i) *
                nb_of_tiles_max_depth) + j}, sizeof(uint32_t)
            );
        }
    }
}

/// @brief Function which calculates all the indexes of right view sorted as
/// would.
///
/// @param buffer The buffer to fill.
/// @param nb_of_tiles_max_depth The maximal number of tile for a depth.
/// @param depth The depth.
static void get_right_indexes(buffer_t PTR buffer,
    uint32_t nb_of_tiles_max_depth, uint32_t depth)
{
    uint32_t n;

    for (uint32_t i = 0; i < depth; i++) {
        n = (nb_of_tiles_max_depth / 2) - i;
        for (uint32_t j = n; j < n + get_nb_of_tiles_for_a_depth(i); j++) {
            append_to_buffer_from_chars(buffer,
                (char *)&(uint32_t){(j * depth) + i}, sizeof(uint32_t)
            );
        }
    }
}

/// @brief Function which calculates all the indexes of left view sorted as
/// would.
///
/// @param buffer The buffer to fill.
/// @param nb_of_tiles_max_depth The maximal number of tile for a depth.
/// @param depth The depth.
static void get_left_indexes(buffer_t PTR buffer,
    uint32_t nb_of_tiles_max_depth, uint32_t depth)
{
    uint32_t n;

    for (uint32_t i = 0; i < depth; i++) {
        n = (nb_of_tiles_max_depth / 2) + i;
        for (int32_t j = (int32_t)n; j > (int32_t)(n -
        get_nb_of_tiles_for_a_depth(i)); j--) {
            append_to_buffer_from_chars(buffer,
                (char *)&(uint32_t){(j * depth) + (depth - i - 1)},
                sizeof(uint32_t)
            );
        }
    }
}

void pre_generate_look_indexes(generated_buffers_t PTR pre_generated_buffers)
{
    uint32_t max_nb_of_tiles = get_total_nb_of_tiles_required(MAX_AI_LVL);
    uint32_t nb_of_tiles_max_depth = get_nb_of_tiles_for_a_depth(MAX_AI_LVL);

    init_buffer(&pre_generated_buffers->buffers[PRE_TOP_INDEXES],
        sizeof(uint32_t) * max_nb_of_tiles);
    init_buffer(&pre_generated_buffers->buffers[PRE_RIGHT_INDEXES],
        sizeof(uint32_t) * max_nb_of_tiles);
    init_buffer(&pre_generated_buffers->buffers[PRE_BOTTOM_INDEXES],
        sizeof(uint32_t) * max_nb_of_tiles);
    init_buffer(&pre_generated_buffers->buffers[PRE_LEFT_INDEXES],
        sizeof(uint32_t) * max_nb_of_tiles);
    get_top_indexes(&pre_generated_buffers->buffers[PRE_TOP_INDEXES],
        nb_of_tiles_max_depth, MAX_AI_LVL + 1);
    get_right_indexes(&pre_generated_buffers->buffers[PRE_RIGHT_INDEXES],
        nb_of_tiles_max_depth, MAX_AI_LVL + 1);
    get_bottom_indexes(&pre_generated_buffers->buffers[PRE_BOTTOM_INDEXES],
        nb_of_tiles_max_depth, MAX_AI_LVL + 1);
    get_left_indexes(&pre_generated_buffers->buffers[PRE_LEFT_INDEXES],
        nb_of_tiles_max_depth, MAX_AI_LVL + 1);
}
