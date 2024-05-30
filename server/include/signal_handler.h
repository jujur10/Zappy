/*
** EPITECH PROJECT, 2024
** signal_handler.h
** File description:
** signal_handler.h.
*/

#pragma once

/// @brief Pipe fds to handle signals and close server properly.
extern int pipe_signals[2];

/// @brief Function that register signals to handle.
void register_signals(void);
