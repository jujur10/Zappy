/*
** EPITECH PROJECT, 2024
** tests_player_inventory_command.c
** File description:
** tests_player_inventory_command.c.
*/
#include <unistd.h>
#include <criterion/criterion.h>

#include "commands/player_commands.h"
#include "server.h"

Test(TEST_PLAYER_INVENTORY_COMMAND, test_player_inventory_command_1)
{
    server_t server = {};
    msg_t message = {};
    char buffer[1 + (sizeof(FOOD_STR) + sizeof(LINEMATE_STR) + sizeof
    (DERAUMERE_STR) + sizeof(SIBUR_STR) + sizeof(MENDIANE_STR) +
    sizeof(PHIRAS_STR) + sizeof(THYSTAME_STR) + (7 * 11)) + 2 + 2];

    TAILQ_INIT(&server.players[0].queue);
    for (uint32_t i = 0; i < R_STRUCT_SIZE - 1; i++)
        server.players[0].inventory.arr[i] = UINT16_MAX;
    execute_player_inventory_command(&server, 0, NULL);
    pop_msg(&server.players[0].queue, &message);
    memcpy(buffer, message.ptr, message.len);
    buffer[message.len] = '\0';
    cr_assert_str_eq(buffer, "[food 65535, linemate 65535, deraumere 65535, "
    "sibur 65535, mendiane 65535, phiras 65535, thystame 65535]\n");
}
