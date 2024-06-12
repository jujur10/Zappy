/*
** EPITECH PROJECT, 2024
** pregenerate.h
** File description:
** pregenerate.h.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "style/status.h"
#include "utils/string/buffer.h"

typedef struct server_s server_t;
typedef struct argument_s argument_t;

/// @brief Structure representing pre-generated buffers.
///
/// @var buffers The generated buffers.
/// @var len The number of generated buffers.
typedef struct generated_buffers_s {
    buffer_t ARRAY buffers;
    uint32_t nb_of_buffer;
} generated_buffers_t;

typedef enum {
    PRE_WELCOME_BUFFER,
    PRE_WORLD_DIM_BUFFER,
    PRE_GENERATED_RESOURCE_COUNTER,
    PRE_TNA_RESPONSE,
    PRE_GENERATED_ARR_LEN
} generated_enum_t;

/// @brief Welcome message.
#define WELCOME_MSG "WELCOME\n"

/// @brief Function to pre-generate responses.
/// @param server The server structure (after initialization).
/// @return True On success, False on failure.
status_t pre_generate_buffers(server_t PTR server);

/// @brief Function to free buffer of pre-generated responses.
void destroy_pre_generated_buffers(
    generated_buffers_t PTR pre_generated_buffers);

/// @brief Function to pre-generate the total required resources structure.
/// @param args The argument structure (after initialization).
/// @param generated_buffers The pre-generated buffers.
void pre_generate_resources_counter(const argument_t PTR args,
    generated_buffers_t PTR pre_generated_buffers);

/// @brief Function to pre-generate the tna response.
/// @param server The server structure.
void set_tna_response(server_t PTR server);
