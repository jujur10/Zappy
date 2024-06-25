/*
** EPITECH PROJECT, 2024
** gui_handling.c
** File description:
** gui_handling.c.
*/
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "queue/msg_queue.h"
#include "logging.h"
#include "commands/gui_commands.h"
#include "gui_handling.h"

int32_t init_gui(server_t PTR server, int sock)
{
    if (MAX_CLIENTS == server->nb_guis)
        return -1;
    LOGF("Swapped to GUI %i", server->nb_guis)
    server->guis[server->nb_guis].sock = sock;
    TAILQ_INIT(&server->guis[server->nb_guis].queue);
    server->nb_guis++;
    return server->nb_guis - 1;
}

void destroy_gui(server_t PTR server, uint32_t gui_idx)
{
    LOGF("Destroying gui (gui idx: %u)", gui_idx)
    FD_CLR(server->guis[gui_idx].sock, &server->current_socks);
    close(server->guis[gui_idx].sock);
    clear_msg_queue(&server->guis[gui_idx].queue);
    server->nb_guis--;
    memmove(&server->guis[gui_idx], &server->guis[server->nb_guis],
    sizeof(gui_t));
    memset(&server->guis[server->nb_guis], 0, sizeof(gui_t));
}

/// @brief Function which execute command if available.
///
/// @param server The server structure.
/// @param gui_idx The gui index.
static void execute_command_if_available(server_t PTR server, uint32_t gui_idx)
{
    gui_command_t next_command;

    if (SUCCESS == get_next_gui_command(&server->guis[gui_idx].command_buffer,
    &next_command)) {
        execute_gui_command(server, (uint16_t)gui_idx, &next_command);
    }
}

/// @brief Function called when the server receive data from a gui_t.\n
/// - Put the client's message into a buffer.\n
/// - If EOF reached, destroy the gui.\n
/// - Else, execute the actions ordered by gui_t.
/// @param server The server pointer.
/// @param gui_idx The gui index of the gui who sent the message.
static void on_gui_rcv(server_t PTR server, uint32_t gui_idx)
{
    char buffer[READ_BUFFER_SIZE];
    int64_t bytes_received = read(server->guis[gui_idx].sock, buffer,
    sizeof(buffer) - 1);
    gui_command_t next_command;

    if (bytes_received < 1) {
        LOG("Gui closed connection")
        return destroy_gui(server, gui_idx);
    }
    LOGF("Gui received : %.*s", (int32_t)bytes_received, buffer)
    gui_command_handling(server, buffer, (uint32_t)bytes_received, gui_idx);
    if (SUCCESS == get_next_gui_command(&server->guis[gui_idx].command_buffer,
    &next_command)) {
        execute_gui_command(server, (uint16_t)gui_idx, &next_command);
    }
}

/// @brief Check the status of the gui.
/// @param server The server pointer.
/// @param gui The gui we want to check the status.
/// @param fd_set The file descriptor set to check if the gui is inside.
/// @return
/// 1 : If the gui is inside the fd set and blocking time has expired.\n
/// 2 : If the gui isn't inside the fd set.
static uint8_t is_ready(gui_t PTR gui, const fd_set PTR fd_set)
{
    if (FD_ISSET(gui->sock, fd_set))
        return 1;
    return 2;
}

/// @brief Function which check and handle if the gui's socket waiting to
/// be read.
/// @param server The server pointer.
/// @param gui_idx The index of the gui we want to check.
/// @param rfds The read file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_guis_rfds(server_t PTR server, uint32_t gui_idx,
    const fd_set PTR rfds, int32_t PTR select_ret)
{
    LOG("Start handle GUI rfds");
    switch (is_ready(&server->guis[gui_idx], rfds)) {
        case 0:
        case 1:
            on_gui_rcv(server, gui_idx);
            (*select_ret)--;
            LOG("Stop handle GUI rfds 2")
            return 1;
        default:
            LOG("Stop handle GUI rfds 3")
            return 0;
    }
}

/// @brief Function which pop message from the queue and send it.
/// @param gui The current gui.
static void send_next_message_from_queue(server_t PTR server, uint32_t gui_idx)
{
    msg_t msg;
    gui_t *gui = &server->guis[gui_idx];

    if (FAILURE == pop_msg(&gui->queue, &msg))
        return;
    LOGF("Send msg from queue (GUI sock %i) : %.*s", gui->sock, msg.len,
    msg.ptr)
    execute_gui_pre_event_function(server, gui_idx, msg.event.gui_event);
    write(gui->sock, msg.ptr, msg.len);
    execute_gui_post_event_function(server, gui_idx, msg.event.gui_event);
    destroy_message(&msg);
}

/// @brief Function which check and handle if the gui's socket waiting to be
/// write.
/// @param server The server pointer.
/// @param gui_idx The index of the gui we want to check.
/// @param wfds The write file descriptor set.
/// @param select_ret The select return value to decrement if an action is
/// made.
/// @return 1 : If an action has been made. 0 : If nothing has been made.
static uint8_t handle_guis_wfds(server_t PTR server, uint32_t gui_idx,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    LOG("Start handle GUI wfds")
    switch (is_ready(&server->guis[gui_idx], wfds)) {
        case 0:
        case 1:
            send_next_message_from_queue(server, gui_idx);
            (*select_ret)--;
            LOG("Stop handle GUI wfds 1")
            return 1;
        default:
            LOG("Stop handle GUI wfds 2")
            return 0;
    }
}

void handle_guis(server_t PTR server, const fd_set PTR rfds,
    const fd_set PTR wfds, int32_t PTR select_ret)
{
    uint16_t count_guis = 0;

    for (int32_t i = 0; count_guis < server->nb_guis &&
    *select_ret > 0 && i < MAX_CLIENTS; i++) {
        LOGF("Handling the actual GUI : %i", i)
        if (0 == server->guis[i].sock)
            continue;
        count_guis++;
        if (FAILURE == queue_empty(&server->guis[i].queue)) {
            handle_guis_wfds(server, i, wfds, select_ret);
            continue;
        } else {
            execute_command_if_available(server, i);
        }
        if (1 == handle_guis_rfds(server, i, rfds, select_ret))
            continue;
        if (1 == handle_guis_wfds(server, i, wfds, select_ret))
            continue;
    }
}
