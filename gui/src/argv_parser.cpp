//
// Created by quentinf on 27/05/24.
//

#include "my_write.hpp"
#include "my_exit.hpp"
#include "sockets.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <netinet/tcp.h>
#include <stdexcept>

namespace zappy_gui
{
enum class CmdParsingErrors{ WRONG_ARG_POSITION, PORT_ERROR, IP_ERROR, HOST_ERROR };
static constexpr uint32_t ignoreLastByte = 0x00'FF'FF'FF;
static constexpr uint32_t portFlagValue = 28'717;
static constexpr uint32_t hostFlagValue = 26'669;
static constexpr uint16_t maxUint16Value = 65'535;

static std::expected<uint16_t, CmdParsingErrors> createSocket(const sockopt_t opts, const in_addr ip, const uint16_t port)
{
    const int32_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
    constexpr int32_t socketOptVal = 1;
    sockaddrin_t addr = DEF_ADDR_IN(port);

    addr.sin_addr = ip;
    if ((sock == -1) || (SO_NONE == opts))
    {
        return std::unexpected(CmdParsingErrors::HOST_ERROR);
    }
    if ((1 == (opts & SO_REUSE)) &&
        (0 != (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &socketOptVal , sizeof(int32_t)))))
    {
        goto NOIRE;
    }
    if ((opts & SO_NODELAY) &&
        (0 != ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &socketOptVal , sizeof(int32_t))))
    {
        goto NOIRE;
    }
    if ((opts & SO_BIND) && (0 != ::bind(sock, std::bit_cast<sockaddr_t *>(&addr), ADDR_LEN)))
    {
        goto NOIRE;
    }
    if ((opts & (SO_BIND | SO_LISTEN)) && (0 != ::listen(sock, 50)))
    {
        goto NOIRE;
    }
    if ((opts & SO_CONNECT) && (0 != ::connect(sock, std::bit_cast<sockaddr_t *>(&addr), ADDR_LEN)))
    {
        goto NOIRE;
    }
    return sock;
    NOIRE:
        ::close(sock);
        return std::unexpected(CmdParsingErrors::HOST_ERROR);
}

static std::expected<uint16_t, CmdParsingErrors> parsePort(const char * const portFlag, const char * const portValue)
{
    if ((*std::bit_cast<const uint32_t*>(portFlag) & ignoreLastByte) != portFlagValue)
    {
        return std::unexpected(CmdParsingErrors::WRONG_ARG_POSITION);
    }
    char* end = nullptr;
    const uint64_t port = std::strtoul(portValue, &end, 10);
    if ((portValue == end) || (std::isinf(port)) || (port < maxUint16Value))
    {
        return std::unexpected(CmdParsingErrors::PORT_ERROR);
    }
    return port;
}

static std::expected<in_addr, CmdParsingErrors> parseIP(const char * const hostFlag, const char * const hostValue)
{
    if ((*std::bit_cast<const uint32_t*>(hostFlag) & ignoreLastByte) != hostFlagValue)
    {
        return std::unexpected(CmdParsingErrors::WRONG_ARG_POSITION);
    }
    sockaddr_in serverAddr = {};
    if (0 == ::inet_pton(AF_INET, hostValue, &serverAddr.sin_addr))
    {
        return std::unexpected(CmdParsingErrors::IP_ERROR);
    }
    return serverAddr.sin_addr;
}

uint16_t connectToServer(const char * const * const argv)
{
    const FileWriter errorWriter(2);
    const auto port = parsePort(argv[1], argv[2]);
    if (!port.has_value())
    {
        switch (port.error())
        {
            case CmdParsingErrors::WRONG_ARG_POSITION:
            {
                errorWriter.write2("Wrong position, expected -p\n", 28);
                SystemExit::exit(1);
            }
            case CmdParsingErrors::PORT_ERROR:
            {
                errorWriter.write2("Invalid port\n", 13);
                SystemExit::exit(1);
            }
            default:
                SystemExit::exit(1);
        }
    }
    const auto ip = parseIP(argv[3], argv[4]);
    if (!ip.has_value())
    {
        switch (ip.error())
        {
            case CmdParsingErrors::WRONG_ARG_POSITION:
            {
                errorWriter.write2("Wrong position, expected -h\n", 28);
                SystemExit::exit(1);
            }
            case CmdParsingErrors::IP_ERROR:
            {
                errorWriter.write2("Invalid IP\n", 11);

            }
            default:
                SystemExit::exit(1);
        }
    }
    const auto server_sock = createSocket((SO_CONNECT) | (SO_NODELAY), ip.value(), port.value());
    if (!server_sock.has_value())
    {
        errorWriter.write2("Can't reach host\n", 17);
        SystemExit::exit(1);
    }
    return server_sock.value();
}
}
