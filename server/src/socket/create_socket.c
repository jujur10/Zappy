/*
** EPITECH PROJECT, 2024
** create_socket.c
** File description:
** Create a socket
*/
#include <netinet/tcp.h>

#include "socket/sockets.h"

int create_socket(sockopt_t opts, ipv4_t ip, uint16_t port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddrin_t addr = DEF_ADDR_IN(port);

    addr.sin_addr.s_addr = htonl(ip.addr);
    if (sock == -1 || SO_NONE == opts)
        return sock;
    if (opts & SO_REUSE &&
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)))
        return close(sock), -1;
    if (opts & SO_NODELAY &&
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &(int){1}, sizeof(int)))
        return close(sock), -1;
    if (opts & SO_BIND && bind(sock, (sockaddr_t *) &addr, ADDR_LEN))
        return close(sock), -1;
    if (opts & (SO_BIND | SO_LISTEN) && listen(sock, 50))
        return close(sock), -1;
    if (opts & SO_CONNECT && connect(sock, (sockaddr_t *) &addr, ADDR_LEN))
        return close(sock), -1;
    return sock;
}
