/*
** EPITECH PROJECT, 2024
** tests_gui_msz_command.c
** File description:
** tests_gui_msz_command.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/gui_commands.h"
#include "server.h"
#include "utils/pre_generate/pre_generate.h"

Test(TEST_GUI_MSZ_COMMAND, test_gui_msz_command_1)
{
    server_t server = {};
    argument_t args;
    msg_t message = {};
    char buffer[200];

    queue_new(&server.guis[0].queue);
    args.width = 10;
    args.height = 10;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    execute_gui_msz_command(&server, 0, NULL);
    queue_pop(&server.guis[0].queue, &message);
    memcpy(buffer, message.ptr, message.len);
    buffer[message.len] = '\0';
    cr_assert_str_eq(buffer, "msz 10 10\n");
}
