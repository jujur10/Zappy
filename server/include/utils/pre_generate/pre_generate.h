/*
** EPITECH PROJECT, 2024
** pregenerate.h
** File description:
** pregenerate.h.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "server.h"
#include "resources.h"
#include "style/status.h"

/// @brief The pre-generated response array.
#define PRE_GENERATED_ARR_LEN 2048
extern char ARRAY pre_generated_responses;

/// @brief Welcome message.
#define WELCOME_MSG "WELCOME\n"
extern uint16_t welcome_msg_off;
extern uint16_t welcome_msg_length;

/// @brief World dimensions message.
extern uint16_t world_dim_off;
extern uint16_t world_dim_length;

/// @brief Function to pre-generate responses.
/// @param server The server structure (after initialization).
/// @return True On success, False on failure.
status_t pre_generate_responses(const server_t PTR server);

/// @brief Function to free buffer of pre-generated responses.
void destroy_pre_generated_responses(void);

/// @brief Structure containing the number of resources for the whole map.
extern resources64_t total_required_resources;

/// @brief Function to pre-generate the total required resources structure.
/// @param args The argument structure (after initialization).
void pre_generate_resources_counter(const argument_t *args);
