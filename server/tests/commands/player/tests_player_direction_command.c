/*
** EPITECH PROJECT, 2024
** tests_player_direction_command.c
** File description:
** tests_player_frequency_command.c.
*/
#include "server.h"

#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/player_commands.h"

Test(TEST_PLAYER_DIRECTION_COMMAND, test_player_direction_command_1)
{
    server_t server = {};
    player_t *player = &server.players[0];
    msg_t message = {};
    char msg_content[50];

    TAILQ_INIT(&player->queue);
    player->orientation = LOOK_RIGHT;
    execute_player_direction_command(&server, 0, NULL);
    pop_msg(&player->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "direction: 1\n");
}
