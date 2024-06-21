/*
** EPITECH PROJECT, 2024
** tests_player_look_command.c
** File description:
** tests_player_look_command.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/player_commands.h"
#include "server.h"

static void check_tile(server_t PTR server, uint32_t map_tile_idx,
    resources_t PTR tile)
{
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++) {
        cr_assert_eq(server->map.tiles[map_tile_idx].arr[i], tile->arr[i]);
    }
}

static void execute_1(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile];

    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    check_tile(server, 24, &sorted_resources[0]);
    check_tile(server, 16, &sorted_resources[1]);
    check_tile(server, 17, &sorted_resources[2]);
    check_tile(server, 18, &sorted_resources[3]);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_1)
{
    server_t server = {};
    argument_t args;
    player_t *player = &server.players[0];

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    player->coordinates.x = (uint16_t)((server.map.width - 1) / 2);
    player->coordinates.y = (uint16_t)(server.map.height - 1);
    execute_1(&server, 0, NULL);
}

static void execute_2(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile];

    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    check_tile(server, 3, &sorted_resources[0]);
    check_tile(server, 11, &sorted_resources[1]);
    check_tile(server, 10, &sorted_resources[2]);
    check_tile(server, 9, &sorted_resources[3]);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_2)
{
    server_t server = {};
    argument_t args;
    player_t *player = &server.players[0];

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    server.players[0].coordinates.x = (uint16_t)((server.map.width - 1) / 2);
    server.players[0].coordinates.y = 0;
    player->orientation = LOOK_BOTTOM;
    execute_2(&server, 0, NULL);
}

static void execute_3(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile];

    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    check_tile(server, 0, &sorted_resources[0]);
    check_tile(server, 22, &sorted_resources[1]);
    check_tile(server, 1, &sorted_resources[2]);
    check_tile(server, 8, &sorted_resources[3]);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_3)
{
    server_t server = {};
    argument_t args;
    player_t *player = &server.players[0];

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    server.players[0].coordinates.x = 0;
    server.players[0].coordinates.y = 0;
    player->orientation = LOOK_RIGHT;
    execute_3(&server, 0, NULL);
}

static void execute_4(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile];

    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    check_tile(server, 0, &sorted_resources[0]);
    check_tile(server, 13, &sorted_resources[1]);
    check_tile(server, 6, &sorted_resources[2]);
    check_tile(server, 27, &sorted_resources[3]);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_4)
{
    server_t server = {};
    argument_t args;
    player_t *player = &server.players[0];

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    server.players[0].coordinates.x = 0;
    server.players[0].coordinates.y = 0;
    player->orientation = LOOK_LEFT;
    execute_4(&server, 0, NULL);
}

static void execute_5(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    player_t *player = &server->players[player_idx];
    uint32_t nb_of_tile = get_total_nb_of_tiles_required(player->level + 1);
    resources_t sorted_resources[nb_of_tile];

    get_sorted_resources(server, player, sorted_resources, nb_of_tile);
    check_tile(server, 27, &sorted_resources[0]);
    check_tile(server, 14, &sorted_resources[1]);
    check_tile(server, 21, &sorted_resources[2]);
    check_tile(server, 0, &sorted_resources[3]);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_5)
{
    server_t server = {};
    argument_t args;
    player_t *player = &server.players[0];

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    server.players[0].coordinates.x = 6;
    server.players[0].coordinates.y = 3;
    player->orientation = LOOK_RIGHT;
    execute_5(&server, 0, NULL);
}

Test(TEST_PLAYER_LOOK_COMMAND, test_player_look_command_6)
{
    server_t server = {};
    argument_t args;
    char msg_content[500];
    msg_t message = {};

    args.width = 7;
    args.height = 4;
    server.args = &args;
    init_map(&args, &server.map);
    pre_generate_buffers(&server);
    spread_resources_on_map(&server.map, &server.generated_buffers);
    server.players[0].coordinates.x = 6;
    server.players[0].coordinates.y = 3;
    TAILQ_INIT(&server.players[0].queue);
    for (uint32_t i = 0; i < R_STRUCT_SIZE; i++) {
        server.map.tiles[server.players[0].coordinates.y * 7 +
            server.players[0].coordinates.x].arr[i] = 50;
    }
    execute_player_look_command(&server, 0, NULL);
    pop_msg(&server.players[0].queue, &message);
    memcpy(msg_content, message.ptr, message.len);
    msg_content[message.len + 1] = '\0';
}
