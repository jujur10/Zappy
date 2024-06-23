/*
** EPITECH PROJECT, 2024
** server_events.c
** File description:
** server_events.c.
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "server.h"
#include "new_clients_handling.h"
#include "logging.h"

void on_connection(server_t PTR server)
{
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);
    int client_sock = accept(server->sock, (struct sockaddr*)&client_address,
    &client_size);

    if (client_sock == -1)
        return;
    if (MAX_CLIENTS == server->nb_clients)
        return (void)close(client_sock);
    server->max_client = client_sock > server->max_client ? client_sock
    : server->max_client;
    FD_SET(client_sock, &server->current_socks);
    server->clients[server->nb_clients].sock = client_sock;
    LOGF("New client initialization (client %hu)", server->nb_clients)
    init_new_client(server, &server->clients[server->nb_clients]);
}
