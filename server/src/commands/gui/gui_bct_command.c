/*
** EPITECH PROJECT, 2024
** gui_bct_command.c
** File description:
** gui_bct_command.c.
*/
#include "gui_handling.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

void execute_gui_bct_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    char buffer[105] = "bct ";
    msg_t message = {};
    const resources_t *current_tile;
    uint32_t count = 4;

    if (!(command->args[0] < server->map.width && command->args[1] <
    server->map.height)) {
        create_message("sbp\n", 4, &message);
        add_msg_to_queue(&server->guis[gui_idx].queue, &message);
        return;
    }
    current_tile = &server->map.tiles
        [(command->args[1] * server->map.width) + command->args[0]];
    write_nb_to_buffer(command->args[0], buffer, &count);
    write_nb_to_buffer(command->args[1], buffer, &count);
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++)
        write_nb_to_buffer(current_tile->arr[i], buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
