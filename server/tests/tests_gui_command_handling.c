/*
** EPITECH PROJECT, 2024
** tests_gui_command_handling.c
** File description:
** tests_gui_command_handling.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/gui_commands.h"
#include "server.h"

void execute_gui_mct_command(__attribute__((unused)) server_t *server,
    __attribute__((unused)) uint16_t gui_idx,
    __attribute__((unused)) const gui_command_t *command)
{
    return;
}

void execute_gui_tna_command(__attribute__((unused)) server_t *server,
    __attribute__((unused)) uint16_t gui_idx,
    __attribute__((unused)) const gui_command_t *command)
{
    return;
}


Test(TEST_GUI_COMMAND_HANDLING, test_gui_command_handling_1)
{
    server_t server = {};

    char buffer[] = "bct 10 500\n";

    gui_command_handling(&server, buffer, 11, 0);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].command,
    GUI_BCT_CMD, "Command : %hhu",
    server.guis[0].command_buffer.commands[0].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[0], 10);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[1], 500);
}

Test(TEST_GUI_COMMAND_HANDLING, test_gui_command_handling_2)
{
    server_t server = {};

    char buffer[] = "bct 10 500\ntna\nppo 5\n";

    gui_command_handling(&server, buffer, 22, 0);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].command,
        GUI_BCT_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[0].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[0], 10);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[1], 500);

    cr_assert_eq(server.guis[0].command_buffer.commands[1].command,
        GUI_TNA_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[1].command);

    cr_assert_eq(server.guis[0].command_buffer.commands[2].command,
        GUI_PPO_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[2].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[2].args[0], 5);
}

Test(TEST_GUI_COMMAND_HANDLING, test_gui_command_handling_3)
{
    server_t server = {};

    char buffer[] = "bct 10 500\n\ntna\nppo 5\n";

    gui_command_handling(&server, buffer, 22, 0);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].command,
        GUI_BCT_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[0].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[0], 10);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[1], 500);

    cr_assert_eq(server.guis[0].command_buffer.commands[1].command,
        GUI_NONE_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[1].command);

    cr_assert_eq(server.guis[0].command_buffer.commands[2].command,
        GUI_TNA_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[2].command);

    cr_assert_eq(server.guis[0].command_buffer.commands[3].command,
        GUI_PPO_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[3].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[3].args[0], 5);

    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.ptr, NULL);
    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.len, 0);
}

Test(TEST_GUI_COMMAND_HANDLING, test_gui_command_handling_4)
{
    server_t server = {};

    char buffer[] = "bct 10 500";
    char buffer2[] = "\ntna\n";

    gui_command_handling(&server, buffer, 10, 0);
    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.len, 10);
    gui_command_handling(&server, buffer2, 5, 0);

    cr_assert_eq(server.guis[0].command_buffer.commands[0].command,
        GUI_BCT_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[0].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[0], 10);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[1], 500);

    cr_assert_eq(server.guis[0].command_buffer.commands[1].command,
        GUI_TNA_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[1].command);

    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.ptr, NULL);
    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.len, 0);
}

Test(TEST_GUI_COMMAND_HANDLING, test_gui_command_handling_5)
{
    server_t server = {};

    char buffer[] = "bct 10 500";
    char buffer2[] = "\ntn\ntna\n";

    gui_command_handling(&server, buffer, 10, 0);
    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.len, 10);
    gui_command_handling(&server, buffer2, 8, 0);

    cr_assert_eq(server.guis[0].command_buffer.commands[0].command,
        GUI_BCT_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[0].command);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[0], 10);
    cr_assert_eq(server.guis[0].command_buffer.commands[0].args[1], 500);

    cr_assert_eq(server.guis[0].command_buffer.commands[1].command,
        GUI_NONE_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[1].command);

    cr_assert_eq(server.guis[0].command_buffer.commands[2].command,
        GUI_TNA_CMD, "Command : %hhu",
        server.guis[0].command_buffer.commands[2].command);

    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.ptr, NULL);
    cr_assert_eq(server.guis[0].command_buffer.raw_buffer.len, 0);
}
