/*
** EPITECH PROJECT, 2024
** gui_commands.h
** File description:
** gui_commands.h.
*/

#pragma once

#include <stdint.h>

#include "utils/string/string.h"
#include "style/macros.h"
#include "events/gui_events.h"

/// @brief Read buffer size in byte.
#define READ_BUFFER_SIZE 256

/// @brief Max number of reallocations.
#define MAX_TEMP_BUFFER_LEN 1024

/// @brief Macros for BCT command.
#define UINT32_MAX_DIGITS 10
#define NB_OF_MAP_AXIS 2
#define BCT_COMMAND_LEN 4

/// @brief Redefinition of structures.
typedef struct gui_s gui_t;
typedef struct server_s server_t;
typedef struct msg_s msg_t;
typedef struct player_s player_t;
typedef struct buffer_s buffer_t;
typedef struct coordinates_s coordinates_t;

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
    GUI_NB_OF_CMD
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
status_t get_next_gui_command(gui_command_buffer_t PTR gui_command_buffer,
    gui_command_t PTR next_command);

/// @brief The gui commands.

/// @brief Function which execute a given command.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
/// @param command The command to execute.
void execute_gui_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function called when unknown command has been made.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
/// @param command The command to execute.
void execute_gui_none_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command);

/// @brief The MSZ command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
/// @param command The command to execute.
void execute_gui_msz_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command);

/// @brief The BCT command implementation.
///
/// @param server The server structure.
/// @param gui The gui who made the request.
/// @param command The command to execute.
void execute_gui_bct_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function used to create the bct message.
///
/// @param server The server structure.
/// @param coordinates The coordinates to the tile to send.
/// @param message The message to create.
/// @return Success if message created as desired behavior, Failure if not.
status_t create_gui_bct_message(server_t PTR server,
    const coordinates_t PTR coordinates, msg_t PTR message);

/// @brief Function which sends to GUIs the events of bct.
///
/// @param server The server structure.
/// @param coordinates The coordinates to the tile to send.
void send_bct_to_guis(server_t PTR server,
    const coordinates_t PTR coordinates);

/// @brief The MCT command implementation.
///
/// @param server The server structure.
/// @param gui The gui who made the request.
/// @param command The command to execute.
void execute_gui_mct_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command);

/// @brief The TNA command implementation.
///
/// @param server The server structure.
/// @param gui The gui who made the request.
/// @param command The command to execute.
void execute_gui_tna_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command);

/// @brief The PPO command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui who made the request.
/// @param command The command to execute.
void execute_gui_ppo_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function which create the PPO message.
///
/// @param server The server structure.
/// @param player_sock The player socket.
/// @param message Message to create.
/// @return Success if message created as desired behavior, Failure if not.
status_t create_gui_ppo_message(server_t PTR server,
    uint16_t player_sock, msg_t PTR message);

/// @brief The PLV command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui who made the request.
/// @param command The command to execute.
void execute_gui_plv_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function which create the PLV message.
///
/// @param server The server structure.
/// @param player_sock The player socket.
/// @param message Message to create.
/// @return Success if message created as desired behavior, Failure if not.
status_t create_gui_plv_message(server_t PTR server,
    uint16_t player_sock, msg_t PTR message);

/// @brief The PIN command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui who made the request.
/// @param command The command to execute.
void execute_gui_pin_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function used to create the pin message.
///
/// @param server The server structure.
/// @param player_sock The player socket.
/// @param message The message to create.
/// @return Success if message created as desired behavior, Failure if not.
status_t create_gui_pin_message(server_t PTR server, uint16_t player_sock,
    msg_t PTR message);

/// @brief Function which sends to GUIs the events of pin.
///
/// @param server The server structure.
/// @param player The player.
/// @param resource_index The resource index.
void send_pin_to_guis(server_t PTR server, const player_t PTR player);

/// @brief The SGT command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui who made the request.
/// @param command The command to execute.
void execute_gui_sgt_command(server_t PTR server, uint16_t gui_idx,
    UNUSED const gui_command_t PTR command);

/// @brief The SST command implementation.
///
/// @param server The server structure.
/// @param gui_idx The gui who made the request.
/// @param command The command to execute.
void execute_gui_sst_command(server_t PTR server, uint16_t gui_idx,
    const gui_command_t PTR command);

/// @brief Function which sends a message to all the guis.
///
/// @param server The server structure.
/// @param ptr The pointer on the message content.
/// @param len The message len.
void send_message_to_guis(server_t PTR server, const char PTR ptr,
    uint32_t len);

/// @brief Function which sends a buffer to all the guis.
///
/// @param server The server structure.
/// @param buffer The buffer to send.
/// @param gui_event The event to set.
void send_buffer_to_guis(server_t PTR server, buffer_t PTR buffer,
    gui_event_t gui_event);

/// @brief Fast responses.

/// @brief Function which adds to the gui queue the suc response.
///
/// @param server The server structure.
/// @param gui The gui to send the message to.
void gui_suc_response(UNUSED const server_t PTR server, gui_t PTR gui);
