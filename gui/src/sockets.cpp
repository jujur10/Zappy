//
// Created by quentinf on 09/06/24.
//

#include "sockets.hpp"

#include <sys/poll.h>
#include <unistd.h>

#include <algorithm>

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

int32_t Socket::GetSocketFd() const
{
    return _sockFd;
}

sockaddr_in Socket::GetAddress() const
{
    return _addr;
}

void Socket::Bind() const
{
    if (::bind(_sockFd, std::bit_cast<sockaddr *>(&_addr), sizeof(_addr)) == -1)
    {
        throw SocketException("Failed to bind socket");
    }
}

int32_t Socket::Accept(sockaddr_in * const clientAddress,
                       socklen_t * const clientAddressLength) const
{
    return ::accept(
        _sockFd, std::bit_cast<sockaddr *>(clientAddress), clientAddressLength);
}

bool Socket::Listen(const int32_t connectionNumber) const
{
    return ::listen(_sockFd, connectionNumber) != -1;
}

bool Socket::Connect(const char *const ip, const uint16_t port)
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

ssize_t Socket::Write(const char *const buffer, const size_t length) const
{
    return ::write(_sockFd, std::bit_cast<void *>(buffer), length);
}

ssize_t Socket::Read(char *const buffer, const size_t length) const
{
    return ::read(_sockFd, std::bit_cast<void *>(buffer), length);
}

ssize_t Socket::ReadWithTimeout(char *const buffer,
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

ssize_t Socket::ReadUntil(std::vector<char> &buffer, const char delimiter) const
{
    ssize_t totalBytesRead = 0;
    const size_t chunkSize = 32;
    const size_t originalSize = buffer.size();

    while (true)
    {
        buffer.resize(buffer.size() + chunkSize);
        const ssize_t bytes_read =
            ::read(_sockFd, buffer.data() + originalSize + totalBytesRead, chunkSize);

        if (-1 == bytes_read)
        {
            buffer.clear();
            return -1;
        }
        if (0 == bytes_read)
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }

        totalBytesRead += bytes_read;

        const auto it = std::find(buffer.begin() + originalSize + totalBytesRead - bytes_read,
                                  buffer.begin() + totalBytesRead + originalSize,
                                  delimiter);
        if (it != buffer.begin() + totalBytesRead + originalSize)
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }
    }

    return totalBytesRead;
}

ssize_t Socket::ReadUntilTimeout(std::vector<char> &buffer,
                                 const char delimiter,
                                 const int32_t timeoutMs) const
{
    ssize_t totalBytesRead = 0;
    const size_t chunkSize = 32;
    const size_t originalSize = buffer.size();

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
            buffer.clear();
            return -1;
        }

        buffer.resize(buffer.size() + chunkSize);
        const ssize_t bytes_read =
            ::read(_sockFd, buffer.data() + originalSize + totalBytesRead, chunkSize);

        if (-1 == bytes_read)
        {
            buffer.clear();
            return -1;
        }
        if (0 == bytes_read)
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }

        totalBytesRead += bytes_read;

        const auto it = std::find(buffer.begin() + originalSize + totalBytesRead - bytes_read,
                                  buffer.begin() + totalBytesRead + originalSize,
                                  delimiter);
        if (it != buffer.begin() + totalBytesRead + originalSize)
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }
    }

    return totalBytesRead;
}

ssize_t Socket::ReadUntilFast(std::vector<char> &buffer, const char delimiter) const
{
    ssize_t totalBytesRead = 0;
    const size_t chunkSize = 2048;
    const size_t originalSize = buffer.size();

    while (true)
    {
        buffer.resize(buffer.size() + chunkSize);
        const ssize_t bytes_read =
            ::recv(_sockFd, buffer.data() + originalSize + totalBytesRead, chunkSize, MSG_DONTWAIT);

        if (0 == bytes_read && (EAGAIN == errno || EWOULDBLOCK == errno))
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }
        if (-1 == bytes_read)
        {
            buffer.clear();
            return -1;
        }


        totalBytesRead += bytes_read;

        const auto it = std::find(buffer.begin() + originalSize + totalBytesRead - bytes_read,
                                  buffer.begin() + totalBytesRead + originalSize,
                                  delimiter);
        if (it != buffer.begin() + totalBytesRead + originalSize || (EAGAIN == errno || EWOULDBLOCK == errno))
        {
            buffer.resize(originalSize + static_cast<size_t>(totalBytesRead));
            break;
        }
    }

    return totalBytesRead;
}

std::string Socket::ReadLineFast(std::vector<char> &buffer) const
{
    auto newlinePos = std::ranges::find(buffer, '\n');

    if (newlinePos == buffer.end())
    {
        const ssize_t bytesRead = ReadUntilFast(buffer, '\n');
        if (-1 == bytesRead)
        {
            return "";
        }

        newlinePos = std::ranges::find(buffer, '\n');
        if (buffer.end() == newlinePos)
        {
            return "";
        }
    }

    std::string line(buffer.begin(), newlinePos);
    buffer.erase(buffer.begin(), newlinePos + 1);
    if (buffer.empty())
    {
        buffer.shrink_to_fit();
    }
    return line;
}

std::string Socket::ReadLineTimeout(std::vector<char> &buffer,
                             const int32_t timeout,
                             std::string &errorMsg) const
{
    auto newlinePos = std::ranges::find(buffer, '\n');

    if (newlinePos == buffer.end())
    {
        const ssize_t bytesRead = ReadUntilTimeout(buffer, '\n', timeout);
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

std::string Socket::ReadLine(std::vector<char> &buffer,
                             std::string &errorMsg) const
{
    auto newlinePos = std::ranges::find(buffer, '\n');

    if (newlinePos == buffer.end())
    {
        const ssize_t bytesRead = ReadUntil(buffer, '\n');
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

void Socket::SetSocketOption(const int32_t level,
                             const int32_t optname,
                             const void *const optval,
                             const socklen_t optlen) const
{
    if (::setsockopt(_sockFd, level, optname, optval, optlen) == -1)
    {
        throw SocketException("Failed to set socket option");
    }
}

void Socket::GetSocketOption(const int32_t level,
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
