/*
** EPITECH PROJECT, 2024
** player_command_parsing.c
** File description:
** player_command_parsing.c.
*/
#include <stdbool.h>
#include <string.h>

#include "style/status.h"
#include "commands/player_commands.h"
#include "ai_handling.h"
#include "logging.h"

/// @brief Function which sets a command and increment the number of commands
/// into the buffer.
///
/// @param player_cmd_buff The player's command buffer.
/// @param cmd_base The command to set.
static void set_base_command(player_command_buffer_t *player_cmd_buff,
    player_command_base_t cmd_base)
{
    player_cmd_buff->commands[player_cmd_buff->nb_of_command].command =
        cmd_base;
    player_cmd_buff->nb_of_command++;
}

/// @brief Function which sets the command parameters when it's only one
/// number.
///
/// @param player_cmd_buff The player's command buffer.
/// @param start_string The pointer on the start of the argument string.
/// @param len The current command length.
static void set_string_parameter(player_command_buffer_t *player_cmd_buff,
    const char ARRAY start_string, uint32_t len)
{
    if (len < 1)
        return set_base_command(player_cmd_buff, PLAYER_NONE_CMD);
    init_string_from_char(&player_cmd_buff->commands[player_cmd_buff
    ->nb_of_command].argument, start_string, len);
}

/// @brief Function which parse the command with variable parameter length.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param player_cmd_buff The player's command buffer.
static void parse_variable_command(const char ARRAY current_ptr, uint32_t len,
    player_command_buffer_t *player_cmd_buff)
{
    if (len >= 5 && 0 == strncmp(current_ptr, PLAYER_SET_OBJ_TXT, sizeof
    (PLAYER_SET_OBJ_TXT))) {
        set_string_parameter(player_cmd_buff, current_ptr + 4, len - 4);
        return set_base_command(player_cmd_buff, PLAYER_SET_OBJ_CMD);
    }
    if (len >= 6 && 0 == strncmp(current_ptr, PLAYER_TAKE_OBJ_TXT, sizeof
    (PLAYER_TAKE_OBJ_TXT))) {
        set_string_parameter(player_cmd_buff, current_ptr + 5, len - 5);
        return set_base_command(player_cmd_buff, PLAYER_TAKE_OBJ_CMD);
    }
    if (len >= 11 && 0 == strncmp(current_ptr, PLAYER_BROADCAST_TXT, sizeof
    (PLAYER_BROADCAST_TXT))) {
        set_string_parameter(player_cmd_buff, current_ptr + 10, len - 10);
        return set_base_command(player_cmd_buff, PLAYER_BROADCAST_CMD);
    }
    return set_base_command(player_cmd_buff, PLAYER_NONE_CMD);
}

/// @brief Function which parse the command with 4 of length.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param player_cmd_buff The player's command buffer.
static void parse_command_of_len_4(const char ARRAY current_ptr,
    player_command_buffer_t *player_cmd_buff)
{
    if (0 == strncmp(current_ptr, PLAYER_LEFT_TXT, sizeof
    (PLAYER_LEFT_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_LEFT_CMD);
    if (0 == strncmp(current_ptr, PLAYER_LOOK_TXT, sizeof
    (PLAYER_LOOK_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_LOOK_CMD);
    if (0 == strncmp(current_ptr, PLAYER_FORK_TXT, sizeof
    (PLAYER_FORK_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_FORK_CMD);
    return set_base_command(player_cmd_buff, PLAYER_NONE_CMD);
}

/// @brief Function which parse the command with 9 and 11 of length.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param player_cmd_buff The player's command buffer.
static void parse_command_of_len_9_11(const char ARRAY current_ptr,
    uint32_t len, player_command_buffer_t *player_cmd_buff)
{
    if (0 == strncmp(current_ptr, PLAYER_INVENTORY_TXT, sizeof
    (PLAYER_INVENTORY_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_INVENTORY_CMD);
    if (0 == strncmp(current_ptr, PLAYER_FREQUENCY_TXT, sizeof
    (PLAYER_FREQUENCY_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_FREQUENCY_CMD);
    if (0 == strncmp(current_ptr, PLAYER_DIRECTION_TXT, sizeof
    (PLAYER_DIRECTION_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_DIRECTION_CMD);
    if (0 == strncmp(current_ptr, PLAYER_CONNECT_NB_TXT, sizeof
    (PLAYER_CONNECT_NB_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_CONNECT_NB_CMD);
    if (0 == strncmp(current_ptr, PLAYER_INCANTATION_TXT, sizeof
    (PLAYER_INCANTATION_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_INCANTATION_CMD);
    return parse_variable_command(current_ptr, len, player_cmd_buff);
}

/// @brief Function which parse the command with 5, 6 and 7 of length.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param player_cmd_buff The player's command buffer.
static void parse_command_of_len_5_6_7(const char ARRAY current_ptr,
    uint32_t len, player_command_buffer_t *player_cmd_buff)
{
    if (0 == strncmp(current_ptr, PLAYER_RIGHT_TXT, sizeof
    (PLAYER_RIGHT_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_RIGHT_CMD);
    if (0 == strncmp(current_ptr, PLAYER_EJECT_TXT, sizeof
    (PLAYER_EJECT_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_EJECT_CMD);
    if (0 == strncmp(current_ptr, PLAYER_FORWARD_TXT, sizeof
    (PLAYER_FORWARD_TXT) - 1))
        return set_base_command(player_cmd_buff, PLAYER_FORWARD_CMD);
    return parse_variable_command(current_ptr, len, player_cmd_buff);
}

/// @brief Function which parse the command and at it to the buffer.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param player_cmd_buff The player's command buffer.
static void parse_command(const char ARRAY current_ptr, uint32_t len,
    player_command_buffer_t *player_cmd_buff)
{
    switch (len) {
        case 4:
            return parse_command_of_len_4(current_ptr, player_cmd_buff);
        case 5:
        case 6:
        case 7:
            return parse_command_of_len_5_6_7(current_ptr, len,
            player_cmd_buff);
        case 9:
        case 11:
            return parse_command_of_len_9_11(current_ptr, len,
            player_cmd_buff);
        default:
            return parse_variable_command(current_ptr, len, player_cmd_buff);
    }
}

/// @brief Function which parses the player's received commands.
///
/// @param buffer The received message content.
/// @param len The current command length.
/// @param player_buffer The player's command buffer.
/// @return Success if string not modified, Failure if modified.
static status_t parse_player_commands(const char ARRAY buffer, uint32_t len,
    player_command_buffer_t *player_buffer, string_t *to_append)
{
    const char *current_ptr = buffer;
    uint32_t current_len;

    while (true) {
        if (MAX_NB_OF_COMMAND_FOR_BUFFER == player_buffer->nb_of_command ||
        current_ptr - buffer >= len)
            return SUCCESS;
        current_len = (uint32_t)strcspn(current_ptr, "\n");
        if ('\0' == current_ptr[current_len]) {
            init_string_from_char(to_append, current_ptr, current_len);
            return FAILURE;
        }
        if (current_len < 4) {
            set_base_command(player_buffer, PLAYER_NONE_CMD);
            current_ptr += current_len + 1;
            continue;
        }
        parse_command(current_ptr, current_len, player_buffer);
        current_ptr += current_len + 1;
    }
}

/// @brief Function which use the player temporary buffer and receive buffer to
/// parse commands.
///
/// @param buffer The received message content.
/// @param len The message length.
/// @param player_buffer The player's command buffer.
/// @return FAILURE if temporary buffer full and command buffer also full,
/// SUCCESS otherwise.
static status_t use_player_buffer(char ARRAY buffer, uint32_t len,
    player_command_buffer_t *player_buffer)
{
    string_t to_append = {};

    if (player_buffer->raw_buffer.len >= MAX_TEMP_BUFFER_LEN) {
        WARN("Limit of buffer allocation reached (buffer is cleared)")
        clear_string(&player_buffer->raw_buffer);
        if (MAX_NB_OF_COMMAND_FOR_BUFFER == player_buffer->nb_of_command)
            return FAILURE;
        set_base_command(player_buffer, PLAYER_NONE_CMD);
        return SUCCESS;
    }
    append_to_string_from_char(&player_buffer->raw_buffer, buffer, len);
    if (FAILURE == parse_player_commands(player_buffer->raw_buffer.ptr,
        player_buffer->raw_buffer.len, player_buffer, &to_append)) {
        clear_string(&player_buffer->raw_buffer);
        player_buffer->raw_buffer = to_append;
        return SUCCESS;
    }
    clear_string(&player_buffer->raw_buffer);
    return SUCCESS;
}

void player_command_handling(server_t *server, char ARRAY buffer, uint32_t len,
    uint32_t player_index)
{
    string_t to_append;
    player_command_buffer_t *player_buffer = &server->players[player_index]
        .command_buffer;

    buffer[len + 1] = '\0';
    len = (uint32_t)strlen(buffer);
    if (len < 4)
        return;
    if (NULL != player_buffer->raw_buffer.ptr) {
        if (FAILURE == use_player_buffer(buffer, len, player_buffer))
            destroy_ai(server, player_index);
        return;
    }
    if (FAILURE == parse_player_commands(buffer, len, player_buffer,
        &to_append)) {
        player_buffer->raw_buffer = to_append;
    }
}
