/*
** EPITECH PROJECT, 2024
** gui_command_parsing.c
** File description:
** gui_command_parsing.c.
*/
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "style/status.h"
#include "commands/gui_commands.h"
#include "gui_handling.h"
#include "logging.h"

/// @brief Function which sets a command and increment the number of commands
/// into the buffer.
///
/// @param gui_cmd_buff The gui's command buffer.
/// @param cmd_base The command to set.
static void set_base_command(gui_command_buffer_t PTR gui_cmd_buff,
    gui_command_base_t cmd_base)
{
    gui_cmd_buff->commands[gui_cmd_buff->nb_of_command].command =
        cmd_base;
    gui_cmd_buff->nb_of_command++;
}

/// @brief Function which sets basic commands (commands without parameters).
///
/// @param command The client's command (casted as uint32_t).
/// @param gui_cmd_buff The gui's command buffer.
static void set_basic_command(uint32_t command,
    gui_command_buffer_t PTR gui_cmd_buff)
{
    switch (command) {
        case GUI_MSZ_U32:
            return set_base_command(gui_cmd_buff, GUI_MSZ_CMD);
        case GUI_MCT_U32:
            return set_base_command(gui_cmd_buff, GUI_MCT_CMD);
        case GUI_TNA_U32:
            return set_base_command(gui_cmd_buff, GUI_TNA_CMD);
        case GUI_SGT_U32:
            return set_base_command(gui_cmd_buff, GUI_SGT_CMD);
        default:
            return;
    }
}

/// @brief Function which sets the command parameters when received command
/// is BCT.
///
/// @param gui_cmd_buff The gui's command buffer.
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
static void set_bct_command_parameters(gui_command_buffer_t PTR gui_cmd_buff,
    const char ARRAY current_ptr, uint32_t len)
{
    uint64_t temp_value;
    char *end_ptr;

    if (len < 5)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    temp_value = strtol(current_ptr + 4, &end_ptr, 10);
    if ((EINVAL == errno || ERANGE == errno) || temp_value >= UINT16_MAX)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    gui_cmd_buff->commands[gui_cmd_buff->nb_of_command].args[0] =
        (uint16_t)temp_value;
    if ((uint64_t)(end_ptr - current_ptr + 1) >= len)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    temp_value = strtol(end_ptr + 1, &end_ptr, 10);
    if ((EINVAL == errno || ERANGE == errno) || temp_value >= UINT16_MAX)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    gui_cmd_buff->commands[gui_cmd_buff->nb_of_command].args[1] =
        (uint16_t)temp_value;
}

/// @brief Function which sets the command parameters when it's only one
/// number.
///
/// @param gui_cmd_buff The gui's command buffer.
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
static void set_num_parameters(gui_command_buffer_t PTR gui_cmd_buff,
    const char ARRAY current_ptr, uint64_t len)
{
    uint64_t temp_value;
    char *end_ptr;

    if (len < 5)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    temp_value = strtol(current_ptr + 4, &end_ptr, 10);
    if ((EINVAL == errno || ERANGE == errno) || temp_value >= UINT16_MAX)
        return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    gui_cmd_buff->commands[gui_cmd_buff->nb_of_command].args[0] =
        (uint16_t)temp_value;
}

/// @brief Function which sets the commands when parameters are expected.
///
/// @param gui_cmd_buff The gui's command buffer.
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
static void set_advanced_command(uint32_t command,
    gui_command_buffer_t PTR gui_cmd_buff, const char ARRAY current_ptr,
    uint32_t len)
{
    switch (command) {
        case GUI_BCT_U32:
            set_bct_command_parameters(gui_cmd_buff, current_ptr, len);
            return set_base_command(gui_cmd_buff, GUI_BCT_CMD);
        case GUI_PPO_U32:
            set_num_parameters(gui_cmd_buff, current_ptr, len);
            return set_base_command(gui_cmd_buff, GUI_PPO_CMD);
        case GUI_PLV_U32:
            set_num_parameters(gui_cmd_buff, current_ptr, len);
            return set_base_command(gui_cmd_buff, GUI_PLV_CMD);
        case GUI_PIN_U32:
            set_num_parameters(gui_cmd_buff, current_ptr, len);
            return set_base_command(gui_cmd_buff, GUI_PIN_CMD);
        case GUI_SST_U32:
            set_num_parameters(gui_cmd_buff, current_ptr, len);
            return set_base_command(gui_cmd_buff, GUI_SST_CMD);
        default:
            return;
    }
}

/// @brief Function which parse the command and at it to the buffer.
/// The current command is casted as uint32_t and the last byte is ignored.
/// In order to obtain the command alias fastly.
///
/// @param current_ptr The pointer on the current command.
/// @param len The current command length.
/// @param gui_cmd_buff The gui's command buffer.
static void parse_command(const char ARRAY current_ptr, uint32_t len,
    gui_command_buffer_t PTR gui_cmd_buff)
{
    uint32_t command = *(const uint32_t *)current_ptr & 0x00FFFFFF;

    switch (command) {
        case GUI_MSZ_U32:
        case GUI_MCT_U32:
        case GUI_TNA_U32:
        case GUI_SGT_U32:
            return set_basic_command(command, gui_cmd_buff);
        case GUI_BCT_U32:
        case GUI_PPO_U32:
        case GUI_PLV_U32:
        case GUI_PIN_U32:
        case GUI_SST_U32:
            return set_advanced_command(command, gui_cmd_buff, current_ptr,
            len);
        default:
            return set_base_command(gui_cmd_buff, GUI_NONE_CMD);
    }
}

/// @brief Function which parses the gui's received commands.
///
/// @param buffer The received message content.
/// @param len The current command length.
/// @param gui_buffer The gui's command buffer.
/// @return Success if string not modified, Failure if modified.
static status_t parse_gui_commands(const char ARRAY buffer, uint32_t len,
    gui_command_buffer_t PTR gui_buffer, string_t PTR to_append)
{
    const char *current_ptr = buffer;
    uint32_t current_len;

    while (true) {
        if (MAX_NB_OF_COMMAND_FOR_BUFFER == gui_buffer->nb_of_command ||
        current_ptr - buffer >= len)
            return SUCCESS;
        current_len = (uint32_t)strcspn(current_ptr, "\n");
        if ('\0' == current_ptr[current_len]) {
            init_string_from_chars(to_append, current_ptr, current_len);
            return FAILURE;
        }
        if (current_len < 3) {
            set_base_command(gui_buffer, GUI_NONE_CMD);
            current_ptr += current_len + 1;
            continue;
        }
        parse_command(current_ptr, current_len, gui_buffer);
        current_ptr += current_len + 1;
    }
}

/// @brief Function which use the gui temporary buffer and receive buffer to
/// parse commands.
///
/// @param buffer The received message content.
/// @param len The message length.
/// @param gui_buffer The gui's command buffer.
/// @return FAILURE if temporary buffer full and command buffer also full,
/// SUCCESS otherwise.
static status_t use_gui_buffer(char ARRAY buffer, uint32_t len,
    gui_command_buffer_t PTR gui_buffer)
{
    string_t to_append = {};

    if (gui_buffer->raw_buffer.len >= MAX_TEMP_BUFFER_LEN) {
        WARN("Limit of buffer allocation reached (buffer is cleared)")
        clear_string(&gui_buffer->raw_buffer);
        if (MAX_NB_OF_COMMAND_FOR_BUFFER == gui_buffer->nb_of_command)
            return FAILURE;
        set_base_command(gui_buffer, GUI_NONE_CMD);
        return SUCCESS;
    }
    append_to_string_from_chars(&gui_buffer->raw_buffer, buffer, len);
    if (FAILURE == parse_gui_commands(gui_buffer->raw_buffer.ptr,
        gui_buffer->raw_buffer.len, gui_buffer, &to_append)) {
        clear_string(&gui_buffer->raw_buffer);
        gui_buffer->raw_buffer = to_append;
        return SUCCESS;
    }
    clear_string(&gui_buffer->raw_buffer);
    return SUCCESS;
}

void gui_command_handling(server_t PTR server, char ARRAY buffer, uint32_t len,
    uint32_t gui_index)
{
    string_t to_append;
    gui_command_buffer_t *gui_buffer = &server->guis[gui_index].command_buffer;

    buffer[len + 1] = '\0';
    len = (uint32_t)strlen(buffer);
    if (NULL != gui_buffer->raw_buffer.ptr) {
        if (FAILURE == use_gui_buffer(buffer, len, gui_buffer))
            destroy_gui(server, gui_index);
        return;
    }
    if (FAILURE == parse_gui_commands(buffer, len, gui_buffer, &to_append)) {
        gui_buffer->raw_buffer = to_append;
    }
}
