/*
** EPITECH PROJECT, 2024
** gui_commands.h
** File description:
** gui_commands.h.
*/

#pragma once

#include <stdint.h>

#include "utils/string/string.h"

/// @brief Read buffer size in byte.
#define READ_BUFFER_SIZE 256

/// @brief Max number of reallocations.
#define MAX_TEMP_BUFFER_LEN 1024

/// @brief Redefinition of structures.
typedef struct gui_s gui_t;
typedef struct server_s server_t;

// Macro used to put enum on 1 byte.
#define PACKED __attribute__ ((packed))

/// @brief Max number of commands into the buffer for GUIs.
#define MAX_NB_OF_COMMAND_FOR_BUFFER 10

/// @brief Enumeration representing the GUI's commands.
///
/// @var GUI_NONE_CMD Representing an empty slot.
/// @var GUI_MSZ_CMD Representing the MSZ command.
/// @var GUI_BCT_CMD Representing the BCT command.
/// @var GUI_MCT_CMD Representing the MCT command.
/// @var GUI_TNA_CMD Representing the TNA command.
/// @var GUI_PPO_CMD Representing the PPO command.
/// @var GUI_PLV_CMD Representing the PLV command.
/// @var GUI_PIN_CMD Representing the PIN command.
/// @var GUI_SGT_CMD Representing the SGT command.
/// @var GUI_SST_CMD Representing the SST command.
typedef enum {
    GUI_NONE_CMD,
    GUI_MSZ_CMD,
    GUI_BCT_CMD,
    GUI_MCT_CMD,
    GUI_TNA_CMD,
    GUI_PPO_CMD,
    GUI_PLV_CMD,
    GUI_PIN_CMD,
    GUI_SGT_CMD,
    GUI_SST_CMD,
} PACKED gui_command_base_t;

/// @brief Enumeration representing the GUI's commands alias as uint32.
///
/// @var GUI_MSZ_U32 Representing the MSZ command as uint32 value.
/// @var GUI_BCT_U32 Representing the BCT command as uint32 value.
/// @var GUI_MCT_U32 Representing the MCT command as uint32 value.
/// @var GUI_TNA_U32 Representing the TNA command as uint32 value.
/// @var GUI_PPO_U32 Representing the PPO command as uint32 value.
/// @var GUI_PLV_U32 Representing the PLV command as uint32 value.
/// @var GUI_PIN_U32 Representing the PIN command as uint32 value.
/// @var GUI_SGT_U32 Representing the SGT command as uint32 value.
/// @var GUI_SST_U32 Representing the SST command as uint32 value.
typedef enum {
    GUI_MSZ_U32 = 8024941,
    GUI_BCT_U32 = 7627618,
    GUI_MCT_U32 = 7627629,
    GUI_TNA_U32 = 6385268,
    GUI_PPO_U32 = 7303280,
    GUI_PLV_U32 = 7761008,
    GUI_PIN_U32 = 7235952,
    GUI_SGT_U32 = 7628659,
    GUI_SST_U32 = 7631731,
} gui_command_u32_t;

/// @brief Structure representing a GUI command.
///
/// @var command The command type.
/// @var args The command arguments.
typedef struct gui_command_s {
    gui_command_base_t command;
    uint16_t args[2];
} gui_command_t;

/// @brief Structure representing the GUI's command buffer.
///
/// @var raw_buffer The raw message buffer in order to stock partial
/// receptions (reception not '\n' terminated).
/// @var nb_of_command The number of commands actually in the 'commands' array.
/// @var commands The stocked commands.
typedef struct gui_command_buffer_s {
    string_t raw_buffer;
    uint8_t nb_of_command;
    gui_command_t commands[MAX_NB_OF_COMMAND_FOR_BUFFER];
} gui_command_buffer_t;

/// @brief Function which effectuate message parsing.
///
/// @param server The server structure.
/// @param buffer The received buffer.
/// @param len The length of the received buffer.
/// @param gui_index The index of the gui in the gui array.
void gui_command_handling(server_t PTR server, char ARRAY buffer, uint32_t len,
    uint32_t gui_index);

/// @brief Function which pop the next command in the next_command variable.
///
/// @param gui_command_buffer The gui command buffer.
/// @param next_command The command popped.
status_t get_next_command(gui_command_buffer_t *gui_command_buffer,
    gui_command_t *next_command);
