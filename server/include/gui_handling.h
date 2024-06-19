/*
** EPITECH PROJECT, 2024
** gui_handling.h
** File description:
** gui_handling.h.
*/

#pragma once

#include "server.h"

/// @brief Function which initializes a gui.
/// @param server The server pointer.
/// @param gui The gui pointer we want to initialize.
/// @param sock The client's socket.
/// @return The gui index in the gui array of -1 if not enough slot.
int32_t init_gui(server_t PTR server, int sock);

/// @brief Function which destroys a gui.
/// @param server The server pointer.
/// @param gui_idx The gui's index of the gui we want to destroy.
/// @param sock The gui's socket.
void destroy_gui(server_t PTR server, uint32_t gui_idx);

/// @brief Function called when a gui is communicating with the server.
/// @param server The server pointer.
/// @param rfds The pointer on the read fd set.
/// @param wfds The pointer on the write fd set.
/// @param select_ret The return value of select.
void handle_guis(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret);

/// @brief Function which executes the commands to initialize the gui.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
void send_starting_guis_events(server_t PTR server, uint16_t gui_idx);
