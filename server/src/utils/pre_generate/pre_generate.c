/*
** EPITECH PROJECT, 2024
** pregenerate.c
** File description:
** Functions to pre-generate response to clients.
*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils/pre_generate/pre_generate.h"
#include "utils/itoa/fast_itoa.h"
#include "server.h"
#include "logging.h"

/// @brief The pre-generated response array.
char PTR pre_generated_responses;

/// @brief Response offsets.
uint16_t welcome_msg_off;
uint16_t welcome_msg_length;
uint16_t world_dim_off;
uint16_t world_dim_length;

/// @brief Set the message and returns the new offset of pre-generated array.
/// @param offset The offset of the pre-generated array to copy to.
/// @return The new offset.
static uint16_t set_welcome(uint16_t offset)
{
    memcpy(pre_generated_responses + offset, WELCOME_MSG,
    sizeof(WELCOME_MSG) - 1);
    welcome_msg_off = offset;
    welcome_msg_length = sizeof(WELCOME_MSG) - 1;
    return offset + sizeof(WELCOME_MSG) - 1;
}

/// @brief Set the message and returns the new offset of pre-generated array.
/// @param offset The offset of the pre-generated array to copy to.
/// @param server The server structure in order to obtain the world dimensions.
/// @return The new offset.
static uint16_t set_world_dimensions(uint16_t offset, const server_t *server)
{
    uint16_t write_offset = (uint16_t)fast_itoa_u32(server->args->width,
    pre_generated_responses + offset);

    pre_generated_responses[offset + write_offset] = ' ';
    write_offset++;
    write_offset += fast_itoa_u32(server->args->height,
    pre_generated_responses + offset + write_offset);
    pre_generated_responses[offset + write_offset] = '\n';
    write_offset++;
    world_dim_off = offset;
    world_dim_length = write_offset;
    return offset + write_offset;
}

uint8_t pre_generate_responses(const server_t *server)
{
    uint16_t offset = 0;

    pre_generated_responses = malloc(sizeof(char) * PRE_GENERATED_ARR_LEN);
    if (NULL == pre_generated_responses)
        return 1;
    offset = set_welcome(offset);
    offset = set_world_dimensions(offset, server);
    pre_generated_responses[offset] = '\0';
    return 0;
}

void destroy_pre_generated_responses(void)
{
    free(pre_generated_responses);
}
