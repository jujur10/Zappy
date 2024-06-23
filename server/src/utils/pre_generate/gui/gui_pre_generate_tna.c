/*
** EPITECH PROJECT, 2024
** gui_pre_generate_tna.c
** File description:
** gui_pre_generate_tna.c.
*/
#include <string.h>
#include "server.h"
#include "utils/string/string.h"
#include "utils/string/buffer.h"

void set_tna_response(server_t PTR server)
{
    string_t tna_response;
    const argument_t *args = server->args;

    init_string_from_chars(&tna_response, "", 0);
    for (uint32_t i = 0; i < args->nb_of_teams; i++) {
        append_to_string_from_chars(&tna_response, "tna ", 4);
        append_to_string_from_chars(&tna_response, args->team_names[i],
        (uint32_t)strlen(args->team_names[i]));
        append_to_string_from_chars(&tna_response, "\n", 1);
    }
    init_buffer_from_chars(
        &server->generated_buffers.buffers[PRE_TNA_RESPONSE],
        tna_response.len, tna_response.ptr, tna_response.len
    );
    clear_string(&tna_response);
}
