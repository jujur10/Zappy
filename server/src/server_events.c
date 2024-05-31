/*
** EPITECH PROJECT, 2024
** server_events.c
** File description:
** server_events.c.
*/
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "server.h"
#include "new_clients_handling.h"

/// @brief Function which is used to get index of a free slot in order to
///  stock the temporary client (new_client_t).
/// @param server The server structure.
/// @return Returns the index of a free slot or -1 on failure.
static int32_t get_free_slot_client(const new_client_t ARRAY clients)
{
    for (int32_t i = 0; i < MAX_CLIENTS; i++)
        if (0 == clients[i].sock)
            return i;
    return -1;
}

void on_connection(server_t *server)
{
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);
    int client_sock = accept(server->sock, (struct sockaddr*)&client_address,
    &client_size);
    int32_t free_slot = get_free_slot_client(server->clients);

    if (client_sock == -1)
        return;
    if (free_slot == -1)
        return (void)close(client_sock);
    server->max_client = client_sock > server->max_client ? client_sock
    : server->max_client;
    FD_SET(client_sock, &server->current_socks);
    server->clients[free_slot].sock = client_sock;
    init_new_client(server, &server->clients[free_slot]);
}
