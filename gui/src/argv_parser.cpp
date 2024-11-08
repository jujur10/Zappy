//
// Created by quentinf on 27/05/24.
//

#include "my_exit.hpp"
#include "my_write.hpp"
#include "sockets.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <netinet/tcp.h>
#include <stdexcept>

namespace zappy_gui
{
/// @brief Enum class for command line parsing errors
enum class CmdParsingErrors
{
    kWRONG_ARG_POSITION,
    kPORT_ERROR,
    kIP_ERROR,
    kHOST_ERROR
};

static constexpr uint32_t ignoreLastByte = 0x00'FF'FF'FF;
static constexpr uint32_t portFlagValue  = 28'717;
static constexpr uint32_t hostFlagValue  = 26'669;

/// @brief Parse the port from the command line arguments
///
/// @param portFlag The flag for the port, should be "-p"
/// @param portValue The value for the port after the flag "-p" as a string
/// @return The port if the parsing was successful, an error otherwise
static std::expected<uint16_t, CmdParsingErrors> ParsePort(const char *const portFlag, const char *const portValue)
{
    if ((*std::bit_cast<const uint32_t *>(portFlag) & ignoreLastByte) != portFlagValue)
    {
        return std::unexpected(CmdParsingErrors::kWRONG_ARG_POSITION);
    }
    char *end           = nullptr;
    const uint64_t port = std::strtoul(portValue, &end, 10);
    if ((portValue == end) || (std::isinf(port)) || (port > UINT16_MAX))
    {
        return std::unexpected(CmdParsingErrors::kPORT_ERROR);
    }
    return port;
}

/// @brief Parse the IP from the command line arguments
///
/// @param hostFlag The flag for the host, should be "-h"
/// @param hostValue The value for the host after the flag "-h" as a string
/// @return The IP if the parsing was successful, an error otherwise
static std::expected<std::string, CmdParsingErrors> ParseIP(const char *const hostFlag, const char *const hostValue)
{
    if ((*std::bit_cast<const uint32_t *>(hostFlag) & ignoreLastByte) != hostFlagValue)
    {
        return std::unexpected(CmdParsingErrors::kWRONG_ARG_POSITION);
    }
    return std::string(hostValue);
}

Socket ConnectToServer(char *const *const argv)
{
    const FileWriter errorWriter(2);
    const auto port = ParsePort(argv[1], argv[2]);
    if (!port.has_value())
    {
        switch (port.error())
        {
            case CmdParsingErrors::kWRONG_ARG_POSITION:
            {
                errorWriter.writeNoReturn("Wrong position, expected -p\n", 28);
                SystemExit::exit(1);
            }
            case CmdParsingErrors::kPORT_ERROR:
            {
                errorWriter.writeNoReturn("Invalid port\n", 13);
                SystemExit::exit(1);
            }
            default: SystemExit::exit(1);
        }
    }
    const auto ip = ParseIP(argv[3], argv[4]);
    if (!ip.has_value())
    {
        switch (ip.error())
        {
            case CmdParsingErrors::kWRONG_ARG_POSITION:
            {
                errorWriter.writeNoReturn("Wrong position, expected -h\n", 28);
                SystemExit::exit(1);
            }
            case CmdParsingErrors::kIP_ERROR:
            {
                errorWriter.writeNoReturn("Invalid IP\n", 11);
                SystemExit::exit(1);
            }
            default: SystemExit::exit(1);
        }
    }
    try
    {
        Socket socket(port.value());
        constexpr int32_t socketOptVal = 1;
        socket.SetSocketOption(IPPROTO_TCP, TCP_NODELAY, &socketOptVal, sizeof(int32_t));
        if (!socket.Connect(ip.value().c_str(), port.value()))
        {
            errorWriter.writeNoReturn("Can't reach host\n", 17);
            SystemExit::exit(1);
        }
        return socket;
    }
    catch (const SocketException &e)
    {
        errorWriter.writeNoReturn(e.what(), std::strlen(e.what()));
        errorWriter.writeNoReturn("\n", 1);
        SystemExit::exit(1);
    }
}
} // namespace zappy_gui
