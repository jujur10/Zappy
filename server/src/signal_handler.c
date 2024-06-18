/*
** EPITECH PROJECT, 2024
** signal_handler.c
** File description:
** signal_handler.c.
*/
#include <signal.h>
#include <unistd.h>

#include "signal_handler.h"
#include "logging.h"
#include "style/macros.h"

/// @brief Function that handle signals.
static void handle_signal(UNUSED int sig)
{
    if (pipe_signals[1] != -1) {
        write(pipe_signals[1], "TERMINATED", 10);
        close(pipe_signals[1]);
        pipe_signals[1] = -1;
        LOG("Close signal received. Waiting to close ...")
    }
}

void register_signals(void)
{
    struct sigaction act = {0};

    act.sa_handler = &handle_signal;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGKILL, &act, NULL);
    signal(SIGPIPE, SIG_IGN);
}
