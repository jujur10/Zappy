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
    PRE_OK_RESPONSE,
    PRE_KO_RESPONSE,
    PRE_WORLD_DIM_BUFFER,
    PRE_GENERATED_RESOURCE_COUNTER,
    PRE_TNA_RESPONSE,
    PRE_MAP_BUFFER,
    PRE_TOP_INDEXES,
    PRE_RIGHT_INDEXES,
    PRE_BOTTOM_INDEXES,
    PRE_LEFT_INDEXES,
    PRE_LOOK_RESPONSE_BUFFER,
    PRE_BROADCAST_MSG,
    PRE_GENERATED_ARR_LEN
} generated_buffer_enum_t;

/// @brief Welcome message.
#define WELCOME_MSG "WELCOME\n"

/// @brief OK response.
#define OK_RESPONSE "ok\n"

/// @brief KO response.
#define KO_RESPONSE "ko\n"

/// @brief Max broadcast message limit.
#define MESSAGE_STR "message K, "
#define MAX_BROADCAST_MSG_LENGTH (sizeof(MESSAGE_STR) + 512)

/// @brief Function to pre-generate responses.
///
/// @param server The server structure (after initialization).
/// @return True On success, False on failure.
status_t pre_generate_buffers(server_t PTR server);

/// @brief Function to free buffer of pre-generated responses.
///
/// @param pre_generated_buffers The allocated buffers to destroy.
void destroy_pre_generated_buffers(
    generated_buffers_t PTR pre_generated_buffers);

/// @brief Function to pre-generate the total required resources structure.
///
/// @param args The argument structure (after initialization).
/// @param generated_buffers The pre-generated buffers.
void pre_generate_resources_counter(const argument_t PTR args,
    generated_buffers_t PTR pre_generated_buffers);

/// @brief Function to pre-generate the tna response.
///
/// @param server The server structure.
void set_tna_response(server_t PTR server);

/// @brief Function to allocate the map buffer.
///
/// @param server The server structure.
void set_mct_buffer(server_t PTR server);

/// @brief Function which generates arrays of sorted indexes in order to
/// obtain the look tiles blazingly fast.
///
/// @param pre_generated_buffers The pre generated buffers structure.
void pre_generate_look_indexes(generated_buffers_t PTR pre_generated_buffers);
