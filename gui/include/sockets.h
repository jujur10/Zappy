/*
** EPITECH PROJECT, 2024
** socks.h
** File description:
** Socks libs
*/

#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define A_FAM .sin_family = AF_INET,
#define A_ADDR .sin_addr = {INADDR_ANY},
#define A_PORT(p) .sin_port = htons(p)

#define DEF_ADDR_IN(p) {.sin_family = AF_INET, .sin_port = htons(p), .sin_addr = {INADDR_ANY}, .sin_zero = {0}}

#define UNUSED __attribute__((unused))

typedef union {
    uint8_t bytes[4];
    uint32_t addr;
} ipv4_t;

typedef struct {
    uint16_t port;
    ipv4_t ip;
} address_t;

typedef sockaddr_in sockaddrin_t;
typedef sockaddr sockaddr_t;

#define ADDR_LEN sizeof(sockaddrin_t)

typedef uint8_t sockopt_t;
/// No options
#define SO_NONE 0
/// Reuse the port / address
#define SO_REUSE 1
/// Bind the socket to a given address and port
#define SO_BIND 1 << 1
/// Listen for incoming connections (server, needs SO_BIND, 50 queue)
#define SO_LISTEN 1 << 2
/// Connect to a given address and port (client)
#define SO_CONNECT 1 << 3
/// Enable the no-delay socket parameter.
#define SO_NODELAY 1 << 4

/// Creates a new socket with the given options.
/// @param opts A bitmask of the options to apply to the socket.
/// @param ip The ip to bind the socket to / connect to (0 if is a server).
/// @param port The port to bind the socket to / connect to.
/// @return The file descriptor of the created socket, -1 on error.
int create_socket(sockopt_t opts, ipv4_t ip, uint16_t port);
