/*
** EPITECH PROJECT, 2024
** gui_pre_generate_mct.c
** File description:
** gui_pre_generate_mct.c.
*/
#include "server.h"

#include "utils/string/buffer.h"

void set_mct_buffer(server_t *server)
{
    uint32_t max_len = ((NB_OF_MAP_AXIS + R_STRUCT_SIZE - 1) *
        (LEN_OF_UINT32_IN_CHAR + 1) + BCT_COMMAND_LEN) *
        (server->map.height * server->map.width) + 1;

    init_buffer(&server->generated_buffers.buffers[PRE_MAP_BUFFER], max_len);
}
