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
#include "server.h"

/// @brief Function that handle signals.
static void handle_signal(UNUSED int sig)
{
    if (pipe_signals[1] != -1) {
        send_quit_server_request();
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
    signal(SIGPIPE, SIG_IGN);
}
