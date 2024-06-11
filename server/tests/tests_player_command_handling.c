/*
** EPITECH PROJECT, 2024
** tests_player_command_handling.c
** File description:
** tests_player_command_handling.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/player_commands.h"
#include "server.h"

Test(TEST_PLAYER_COMMAND_HANDLING, test_player_command_handling_1)
{
    server_t server = {};

    char buffer[] = "Forward\nRight\nBroadcast sex\n";

    player_command_handling(&server, buffer, 28, 0);
    cr_assert_eq(server.players[0].command_buffer.commands[0].command,
    PLAYER_FORWARD_CMD, "Command : %hhu",
    server.players[0].command_buffer.commands[0].command);
    cr_assert_eq(server.players[0].command_buffer.commands[1].command,
    PLAYER_RIGHT_CMD, "Command : %hhu",
    server.players[0].command_buffer.commands[1].command);
    cr_assert_eq(server.players[0].command_buffer.commands[2].command,
    PLAYER_BROADCAST_CMD, "Command : %hhu",
    server.players[0].command_buffer.commands[2].command);
    cr_assert_str_eq(server.players[0].command_buffer.commands[2].argument
    .ptr, "sex");
}
