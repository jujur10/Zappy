/*
** EPITECH PROJECT, 2024
** tests_gui_bct_command.c
** File description:
** tests_gui_bct_command.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/gui_commands.h"
#include "server.h"
#include "utils/pre_generate/pre_generate.h"

Test(TEST_GUI_BCT_COMMAND, test_gui_bct_command_1)
{
    server_t server = {};
    gui_command_t command = {GUI_BCT_CMD, {10, 10}};
    argument_t args;
    msg_t message;
    char buffer[200];

    args.width = 20;
    args.height = 20;
    init_map(&args, &server.map);
    pre_generate_resources_counter(&args);
    spread_resources_on_map(&server.map);
    TAILQ_INIT(&server.guis[0].queue);
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.foods = 10;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.linemates = 20;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.deraumeres = 30;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.siburs = 40;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.mendianes = 50;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.phiras = 60;
    server.map.tiles[(command.args[1] * server.map.width) + command
        .args[0]].attr.thystames = 70;
    execute_gui_bct_command(&server, &server.guis[0], &command);
    pop_msg(&server.guis[0].queue, &message);
    memcpy(buffer, message.ptr, message.len);
    buffer[message.len] = '\0';
    cr_assert_str_eq(buffer, "bct 10 10 10 20 30 40 50 60 70\n");
}
