/*
** EPITECH PROJECT, 2024
** gui_pin_command.c
** File description:
** gui_pin_command.c.
*/
#include "gui_handling.h"
#include "commands/command_utils.h"

status_t create_gui_pin_message(server_t PTR server, uint16_t player_sock,
    msg_t PTR message)
{
    char buffer[4 + (UINT32_MAX_DIGITS * (3 + INVENTORY_SIZE) + 1)] = "pin ";
    uint32_t count = 4;
    int32_t player_idx = get_player_by_socket(server, player_sock);
    const player_t *player = &server->players[player_idx];

    if (-1 == player_idx) {
        create_message("sbp\n", 4, message);
        return FAILURE;
    }
    write_nb_to_buffer(player->sock, buffer, &count);
    write_nb_to_buffer(player->coordinates.x, buffer, &count);
    write_nb_to_buffer(player->coordinates.y, buffer, &count);
    for (uint32_t i = 0; i < INVENTORY_SIZE; i++)
        write_nb_to_buffer(player->inventory.arr[i], buffer, &count);
    buffer[count - 1] = '\n';
    create_message(buffer, count, message);
    return SUCCESS;
}

void send_pin_to_guis(server_t PTR server, const player_t PTR player)
{
    msg_t message = {};

    if (SUCCESS == create_gui_pin_message(server, player->sock, &message))
        send_message_to_guis(server, message.ptr, message.len);
    destroy_message(&message);
}

void execute_gui_pin_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command)
{
    msg_t message = {};

    create_gui_pin_message(server, command->args[0], &message);
    add_msg_to_queue(&server->guis[gui_idx].queue, &message);
}
