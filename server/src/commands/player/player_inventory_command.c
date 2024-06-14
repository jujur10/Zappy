/*
** EPITECH PROJECT, 2024
** player_inventory_command.c
** File description:
** player_inventory_command.c.
*/
#include <string.h>
#include "server.h"

#include "queue/msg_queue.h"
#include "commands/command_utils.h"
#include "utils/itoa/fast_itoa.h"

/// @brief Constant representing the max buffer size for the string
/// representation of the inventory.
static const uint32_t INVENTORY_STRING_MAX_LENGTH = 1 + (sizeof(FOOD_STR) +
    sizeof(LINEMATE_STR) + sizeof(DERAUMERE_STR) + sizeof(SIBUR_STR) +
    sizeof(MENDIANE_STR) + sizeof(PHIRAS_STR) + sizeof(THYSTAME_STR) +
    (7 * 11)) + 2 + 2;

/// @brief Function which write into the buffer a string and a number.
///
/// @param buffer The buffer to fill.
/// @param string The string to insert into the buffer.
/// @param len The length of the string.
/// @param nb The number to write into the buffer
/// @return The length of the written segment.
static uint32_t write_string_with_nb(char ARRAY buffer,
    const char ARRAY string, uint64_t len, uint16_t nb)
{
    uint32_t wrote = 0;

    memcpy(buffer, string, len);
    wrote += len;
    buffer[wrote] = ' ';
    wrote++;
    wrote += fast_itoa_u32(nb, buffer + wrote);
    memcpy(buffer + wrote, ", ", 2);
    return wrote + 2;
}

/// @brief Function which write into the buffer in parameter an inventory.
///
/// @param buffer The buffer to modify.
/// @param inventory The inventory we want to write into the buffer.
/// @return The length of the written segment.
static uint32_t write_inventory_as_chars(char ARRAY buffer,
    const inventory_t PTR inventory)
{
    uint32_t wrote = 0;

    buffer[wrote] = '[';
    wrote++;
    wrote += write_string_with_nb(buffer + wrote, FOOD_STR, sizeof
    (FOOD_STR) - 1, inventory->attr.food);
    wrote += write_string_with_nb(buffer + wrote, LINEMATE_STR, sizeof
    (LINEMATE_STR) - 1, inventory->attr.linemate);
    wrote += write_string_with_nb(buffer + wrote, DERAUMERE_STR, sizeof
    (DERAUMERE_STR) - 1, inventory->attr.deraumere);
    wrote += write_string_with_nb(buffer + wrote, SIBUR_STR, sizeof
    (SIBUR_STR) - 1, inventory->attr.sibur);
    wrote += write_string_with_nb(buffer + wrote, MENDIANE_STR, sizeof
    (MENDIANE_STR) - 1, inventory->attr.mendiane);
    wrote += write_string_with_nb(buffer + wrote, PHIRAS_STR, sizeof
    (PHIRAS_STR) - 1, inventory->attr.phiras);
    wrote += write_string_with_nb(buffer + wrote, THYSTAME_STR, sizeof
    (THYSTAME_STR) - 1, inventory->attr.thystame);
    buffer[wrote - 2] = ']';
    return wrote;
}

void execute_player_inventory_command(server_t PTR server, uint16_t player_idx,
    UNUSED const player_command_t PTR command)
{
    char buffer[INVENTORY_STRING_MAX_LENGTH];
    const inventory_t *inventory = &server->players[player_idx].inventory;
    uint32_t wrote;
    msg_t message;

    wrote = write_inventory_as_chars(buffer, inventory);
    create_message(buffer, wrote - 1, &message);
    add_msg_to_queue(&server->players[player_idx].queue, &message);
}
