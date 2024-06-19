/*
** EPITECH PROJECT, 2024
** gui.c
** File description:
** gui.c.
*/
#include <string.h>
#include "server.h"
#include "commands/command_utils.h"

/// @brief Function which sends to a GUI the events of enw.
///
/// @param server The server structure.
/// @param egg The egg.
/// @param gui_idx The gui index.
static void send_enw_to_gui(server_t PTR server, const egg_t PTR egg,
    uint16_t gui_idx)
{
    msg_t message;
    char msg_content[4 + (4 * UINT32_MAX_DIGITS) + 1] = "enw ";
    uint32_t count = 4;

    write_nb_to_buffer(egg->index, msg_content, &count);
    memcpy(msg_content + count, "-1 ", 3);
    count += 3;
    write_nb_to_buffer(egg->egg_coordinates.x, msg_content, &count);
    write_nb_to_buffer(egg->egg_coordinates.y, msg_content, &count);
    msg_content[count - 1] = '\n';
    create_message(msg_content, count, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}

void send_starting_guis_events(server_t PTR server, uint16_t gui_idx)
{
    execute_gui_tna_command(server, gui_idx, NULL);
    for (uint16_t team_idx = 0; team_idx < server->args->nb_of_teams;
        team_idx++) {
        for (uint16_t egg_idx = 0; egg_idx <
            server->teams[team_idx].nb_of_eggs; egg_idx++)
                send_enw_to_gui(server, &server->teams[team_idx]
                .eggs[egg_idx], gui_idx);
    }
}
