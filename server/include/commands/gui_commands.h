/*
** EPITECH PROJECT, 2024
** gui_commands.h
** File description:
** gui_commands.h.
*/

#pragma once

#include <stdint.h>

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
    char ARRAY raw_buffer;
    uint8_t nb_of_command;
    gui_command_t commands[MAX_NB_OF_COMMAND_FOR_BUFFER];
} gui_command_buffer_t;
