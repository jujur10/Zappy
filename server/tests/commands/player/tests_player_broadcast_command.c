/*
** EPITECH PROJECT, 2024
** tests_player_broadcast_command.c
** File description:
** tests_player_broadcast_command.c.
*/
#include "server.h"

#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/player_commands.h"

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_pdf_exemple)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 10;
    server.map.height = 11;
    server.nb_players = 2;
    player2->orientation = LOOK_RIGHT;
    player1->coordinates.x = 2;
    player1->coordinates.y = 9;
    player2->coordinates.x = 5;
    player2->coordinates.y = 2;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 4, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_out_of_map)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 1;
    player2->coordinates.x = 7;
    player2->coordinates.y = 7;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 6, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_same_tile)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 1;
    player2->coordinates.x = 1;
    player2->coordinates.y = 1;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 0, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_left)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 1;
    player2->coordinates.x = 0;
    player2->coordinates.y = 1;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 7, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_right)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 1;
    player2->coordinates.x = 2;
    player2->coordinates.y = 1;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 3, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_top)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 0;
    player2->coordinates.x = 1;
    player2->coordinates.y = 1;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 1, je m'appelle toto\n");
}

Test(TEST_PLAYER_BROADCAST_COMMAND, test_player_broadcast_command_bottom)
{
    server_t server = {};
    player_t *player1 = &server.players[0];
    player_t *player2 = &server.players[1];
    string_t string;
    argument_t args;
    char msg_content[50];
    msg_t message = {};

    queue_new(&server.players[0].queue);
    queue_new(&server.players[1].queue);
    args.width = 8;
    args.height = 8;
    server.args = &args;
    init_string_from_chars(&string, "je m'appelle toto", 17);
    player_command_t cmd = {PLAYER_NONE_CMD, string};
    pre_generate_buffers(&server);
    server.map.width = 8;
    server.map.height = 8;
    server.nb_players = 2;
    player2->orientation = LOOK_TOP;
    player1->coordinates.x = 1;
    player1->coordinates.y = 2;
    player2->coordinates.x = 1;
    player2->coordinates.y = 1;
    execute_player_broadcast_command(&server, 0, &cmd);
    queue_pop(&player2->queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
    cr_assert_str_eq(msg_content, "message 5, je m'appelle toto\n");
}
