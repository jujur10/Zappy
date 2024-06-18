/*
** EPITECH PROJECT, 2024
** gui_pin_command.c
** File description:
** gui_pin_command.c.
*/
#include "utils/itoa/fast_itoa.h"
#include "gui_handling.h"
#include "commands/command_utils.h"

void execute_gui_pin_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    char buffer[120] = "pin ";
    msg_t message = {};
    uint32_t count = 4;
    int32_t player_idx = get_player_by_socket(server, command->args[0]);
    const player_t *current_player;

    if (-1 == player_idx) {
        create_message("sbp\n", 4, &message);
        add_msg_to_queue(&server->guis[gui_idx].queue, &message);
        return;
    }
    current_player = &server->players[player_idx];
    write_nb_to_buffer(command->args[0], buffer, &count);
    write_nb_to_buffer(current_player->coordinates.x, buffer, &count);
    write_nb_to_buffer(current_player->coordinates.y, buffer, &count);
    for (uint32_t i = 0; i < INVENTORY_SIZE; i++)
        write_nb_to_buffer(current_player->inventory.arr[i], buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
