/*
** EPITECH PROJECT, 2024
** gui_bct_command.c
** File description:
** gui_bct_command.c.
*/
#include "gui_handling.h"
#include "utils/itoa/fast_itoa.h"
#include "commands/command_utils.h"

status_t create_gui_bct_message(server_t PTR server,
    const coordinates_t PTR coordinates, msg_t PTR message)
{
    char buffer[4 + (UINT32_MAX_DIGITS * (2 + R_STRUCT_SIZE - 1) + 1)] =
        "bct ";
    const resources_t *current_tile;
    uint32_t count = 4;

    if (!(coordinates->x < server->map.width && coordinates->y <
    server->map.height)) {
        create_message("sbp\n", 4, message);
        return FAILURE;
    }
    current_tile = get_resource_tile_by_coordinates(&server->map, coordinates);
    write_nb_to_buffer(coordinates->x, buffer, &count);
    write_nb_to_buffer(coordinates->y, buffer, &count);
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++)
        write_nb_to_buffer(current_tile->arr[i], buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, message);
    return SUCCESS;
}

void send_bct_to_guis(server_t PTR server, const coordinates_t PTR coordinates)
{
    msg_t message = {};

    if (SUCCESS == create_gui_bct_message(server, coordinates, &message))
        send_message_to_guis(server, message.ptr, message.len);
    destroy_message(&message);
}

void execute_gui_bct_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    msg_t message = {};
    coordinates_t coordinates = {command->args[0], command->args[1]};

    create_gui_bct_message(server, &coordinates, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
