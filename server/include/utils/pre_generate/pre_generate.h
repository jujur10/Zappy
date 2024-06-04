/*
** EPITECH PROJECT, 2024
** pregenerate.h
** File description:
** pregenerate.h.
*/

#pragma once

#include <stdint.h>

#include "server.h"

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
/// @return 0 On success, 1 on failure.
uint8_t pre_generate_responses(const server_t PTR server);

/// @brief Function to free buffer of pre-generated responses.
void destroy_pre_generated_responses(void);
