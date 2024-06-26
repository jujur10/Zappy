/*
** EPITECH PROJECT, 2024
** player_frequency_command.c
** File description:
** player_frequency_command.c.
*/
#include "server.h"

#include "utils/itoa/fast_itoa.h"

void execute_player_frequency_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    char msg_content[sizeof("frequency: ") + sizeof("4294967295")] =
        {"frequency: "};
    player_t *player = &server->players[player_idx];
    msg_t message = {};
    uint64_t wrote;

    wrote = fast_itoa_u32(server->frequency, msg_content +
        sizeof("frequency: ") - 1);
    msg_content[sizeof("frequency: ") - 1 + wrote] = '\n';
    create_message(msg_content, (uint32_t)(sizeof("frequency: ") + wrote),
        &message);
    queue_push(&player->queue, &message);
}
