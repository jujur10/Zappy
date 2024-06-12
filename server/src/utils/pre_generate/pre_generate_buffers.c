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
#include "style/status.h"
#include "utils/string/buffer.h"

/// @brief Set the message and returns the new offset of pre-generated array.
/// @param offset The offset of the pre-generated array to copy to.
/// @return The new offset.
static void set_welcome(generated_buffers_t PTR pre_generated_buffers)
{
    init_buffer_from_chars(&pre_generated_buffers->buffers[PRE_WELCOME_BUFFER
    ], sizeof(WELCOME_MSG), WELCOME_MSG, sizeof(WELCOME_MSG) - 1);
}

/// @brief Set the message and returns the new offset of pre-generated array.
/// @param offset The offset of the pre-generated array to copy to.
/// @param server The server structure in order to obtain the world dimensions.
/// @return The new offset.
static void set_world_dimensions(const server_t PTR server,
    generated_buffers_t PTR pre_generated_buffers)
{
    char buffer[100];
    uint16_t write_offset = (uint16_t)fast_itoa_u32(server->args->width,
    buffer);

    buffer[write_offset] = ' ';
    write_offset++;
    write_offset += fast_itoa_u32(server->args->height, buffer + write_offset);
    buffer[write_offset] = '\n';
    write_offset++;
    init_buffer_from_chars
    (&pre_generated_buffers->buffers[PRE_WORLD_DIM_BUFFER], write_offset + 1,
    buffer, write_offset);
}

status_t pre_generate_buffers(server_t PTR server)
{
    server->generated_buffers.buffers = malloc(sizeof(buffer_t) *
        PRE_GENERATED_ARR_LEN);
    if (NULL == server->generated_buffers.buffers)
        return FAILURE;
    set_welcome(&server->generated_buffers);
    set_world_dimensions(server, &server->generated_buffers);
    pre_generate_resources_counter(server->args, &server->generated_buffers);
    server->generated_buffers.nb_of_buffer = PRE_GENERATED_ARR_LEN;
    return SUCCESS;
}

void destroy_pre_generated_buffers(
    generated_buffers_t PTR pre_generated_buffers)
{
    for (uint32_t i = 0; i < pre_generated_buffers->nb_of_buffer; i++)
        destroy_buffer(&pre_generated_buffers->buffers[i]);
    free(pre_generated_buffers->buffers);
}
