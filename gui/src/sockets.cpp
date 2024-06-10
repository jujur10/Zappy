//
// Created by quentinf on 09/06/24.
//
#include "sockets.hpp"

namespace zappy_gui
{
SocketException::SocketException(std::string message)
    : errorMessage(std::move(message))
{
}

const char *SocketException::what() const noexcept
{
    return errorMessage.c_str();
}

Socket::Socket(const uint16_t port)
{
    _sockFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_sockFd == -1)
    {
        throw SocketException("Failed to create socket");
    }

    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = ::htons(port);
}

Socket::~Socket()
{
    ::close(_sockFd);
}

int32_t Socket::getSocketFd() const
{
    return _sockFd;
}

sockaddr_in Socket::getAddress() const
{
    return _addr;
}

void Socket::bind() const
{
    if (::bind(_sockFd, std::bit_cast<sockaddr *>(&_addr), sizeof(_addr)) == -1)
    {
        throw SocketException("Failed to bind socket");
    }
}

int32_t Socket::accept(sockaddr_in *clientAddress,
                       socklen_t *clientAddressLength) const
{
    return ::accept(
        _sockFd, std::bit_cast<sockaddr *>(clientAddress), clientAddressLength);
}

bool Socket::listen(const int32_t connectionNumber) const
{
    return ::listen(_sockFd, connectionNumber) != -1;
}

bool Socket::connect(const char *const ip, const uint16_t port)
{
    _addr.sin_addr.s_addr = ::inet_addr(ip);

    if (INADDR_NONE == _addr.sin_addr.s_addr)
    {
        throw SocketException("Invalid IP address");
    }

    _addr.sin_port = ::htons(port);
    if (::connect(_sockFd, std::bit_cast<sockaddr *>(&_addr), sizeof(_addr)) ==
        -1)
    {
        throw SocketException("Failed to connect to server");
    }
    return true;
}

ssize_t Socket::write(const char *const buffer, const size_t length) const
{
    return ::write(_sockFd, std::bit_cast<void *>(buffer), length);
}

ssize_t Socket::read(char *const buffer, const size_t length) const
{
    return ::read(_sockFd, std::bit_cast<void *>(buffer), length);
}

ssize_t Socket::readWithTimeout(char *const buffer,
                                const size_t length,
                                const int32_t timeoutMs) const
{
    pollfd pfd{.fd = _sockFd, .events = POLLIN, .revents = 0};
    const int32_t ret = ::poll(&pfd, 1, timeoutMs);

    if (-1 == ret)
    {
        // Error occurred
        return -1;
    }
    if (0 == ret)
    {
        // Timeout reached
        errno = ETIMEDOUT;
        return -1;
    }

    // Socket is ready for reading
    return ::read(_sockFd, std::bit_cast<void *>(buffer), length);
}

ssize_t Socket::readUntil(std::vector<char> &buffer, const char delimiter) const
{
    ssize_t totalBytesRead = 0;
    const size_t chunkSize = 32;
    buffer.clear();

    while (true)
    {
        buffer.resize(buffer.size() + chunkSize);
        const ssize_t bytesRead =
            ::read(_sockFd, buffer.data() + totalBytesRead, chunkSize);

        if (-1 == bytesRead)
        {
            buffer.clear();
            return -1;
        }
        if (0 == bytesRead)
        {
            buffer.resize(static_cast<size_t>(totalBytesRead));
            break;
        }

        totalBytesRead += bytesRead;

        const auto it = std::find(buffer.begin() + totalBytesRead - bytesRead,
                                  buffer.begin() + totalBytesRead,
                                  delimiter);
        if (it != buffer.begin() + totalBytesRead)
        {
            buffer.resize(static_cast<size_t>(totalBytesRead));
            break;
        }
    }
    return totalBytesRead;
}

ssize_t Socket::readUntilTimeout(std::vector<char> &buffer,
                                 const char delimiter,
                                 const int32_t timeoutMs) const
{
    ssize_t totalBytesRead = 0;
    const size_t chunkSize = 32;
    buffer.clear();

    while (true)
    {
        pollfd pfd{.fd = _sockFd, .events = POLLIN, .revents = 0};

        const int32_t ret = ::poll(&pfd, 1, timeoutMs);

        if (-1 == ret)
        {
            buffer.clear();
            return -1;
        }
        if (0 == ret)
        {
            errno = ETIMEDOUT;
            buffer.resize(static_cast<size_t>(totalBytesRead));
            return -1;
        }

        buffer.resize(buffer.size() + chunkSize);
        const ssize_t bytes_read =
            ::read(_sockFd, buffer.data() + totalBytesRead, chunkSize);

        if (-1 == bytes_read)
        {
            buffer.clear();
            return -1;
        }
        if (0 == bytes_read)
        {
            buffer.resize(static_cast<size_t>(totalBytesRead));
            break;
        }

        totalBytesRead += bytes_read;

        const auto it = std::find(buffer.begin() + totalBytesRead - bytes_read,
                                  buffer.begin() + totalBytesRead,
                                  delimiter);
        if (it != buffer.begin() + totalBytesRead)
        {
            buffer.resize(static_cast<size_t>(totalBytesRead));
            break;
        }
    }

    return totalBytesRead;
}

std::string Socket::readLine(std::vector<char> &buffer,
                             const int32_t timeout,
                             std::string &errorMsg) const
{
    auto newlinePos = std::ranges::find(buffer, '\n');

    if (newlinePos == buffer.end())
    {
        const ssize_t bytesRead = readUntilTimeout(buffer, '\n', timeout);
        if (-1 == bytesRead)
        {
            errorMsg = "Failed to read line from socket, timeout or no \\n";
            return "";
        }

        newlinePos = std::ranges::find(buffer, '\n');
        if (buffer.end() == newlinePos)
        {
            errorMsg = "Failed to find \\n in the received data";
            return "";
        }
    }

    std::string line(buffer.begin(), newlinePos);
    buffer.erase(buffer.begin(), newlinePos + 1);
    return line;
}

void Socket::setSocketOption(const int32_t level,
                             const int32_t optname,
                             const void *const optval,
                             const socklen_t optlen) const
{
    if (::setsockopt(_sockFd, level, optname, optval, optlen) == -1)
    {
        throw SocketException("Failed to set socket option");
    }
}

void Socket::getSocketOption(const int32_t level,
                             const int32_t optname,
                             void *const optval,
                             socklen_t *const optlen) const
{
    if (::getsockopt(_sockFd, level, optname, optval, optlen) == -1)
    {
        throw SocketException("Failed to get socket option");
    }
}

}  // namespace zappy_gui
