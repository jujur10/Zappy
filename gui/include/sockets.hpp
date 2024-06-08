#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <cstdint>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace zappy_gui
{
/**
 * @class SocketException
 * Exception class for socket-related errors.
 *
 * This class is derived from std::exception and is used to throw exceptions
 * related to socket operations.
 */
class SocketException final : public std::exception
{
public:
    /**
     * Constructor for SocketException.
     * @param message The error message string.
     */
    explicit SocketException(std::string message) : errorMessage(std::move(message)) {}

    /**
     * Returns the error message string.
     * @return The error message as a C-style string.
     */
    [[nodiscard]]
    const char *what() const noexcept override
    {
        return errorMessage.c_str();
    }

private:
    std::string errorMessage;
};

/**
 * @class Socket
 * Socket class for TCP/IP communication.
 *
 * This class provides functionality for creating a socket, connecting to a server,
 * reading from and writing to the socket, and setting/getting socket options.
 */
class Socket
{
public:
    /**
     * Constructor for Socket.
     * @param port The port number to bind the socket to.
     * @throw SocketException If socket creation fails.
     */
    explicit Socket(const uint16_t port)
    {
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            throw SocketException("Failed to create socket");
        }

        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(port);
    }

    /**
     * @brief Destructor for Socket.
     *
     * Closes the socket.
     */
    ~Socket()
    {
        ::close(sockfd);
    }

    /**
     * Connects the socket to a server.
     * @param ip The IP address of the server.
     * @param port The port number of the server.
     * @return true if the connection is successful, false otherwise.
     * @throw SocketException If the IP address is invalid or connection fails.
     */
    bool connect(const char *ip, const uint16_t port)
    {
        addr.sin_addr.s_addr = inet_addr(ip);
        if (addr.sin_addr.s_addr == INADDR_NONE)
        {
            throw SocketException("Invalid IP address");
        }
        addr.sin_port = htons(port);
        if (::connect(sockfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
        {
            throw SocketException("Failed to connect to server");
        }
        return true;
    }

    /**
     * Writes data to the socket.
     * @param buffer The buffer containing the data to write.
     * @param length The length of the data to write.
     * @return The number of bytes written, or -1 if an error occurs.
     */
    ssize_t write(const void *buffer, const size_t length) const
    {
        return ::write(sockfd, buffer, length);
    }

    /**
     * Reads data from the socket.
     * @param buffer The buffer to store the read data.
     * @param length The maximum length of data to read.
     * @return The number of bytes read, or -1 if an error occurs.
     */
    ssize_t read(void *buffer, const size_t length) const
    {
        return ::read(sockfd, buffer, length);
    }

    /**
     * Reads data from the socket with a timeout.
     * @param buffer The buffer to store the read data.
     * @param length The maximum length of data to read.
     * @param timeout_ms The timeout value in milliseconds.
     * @return The number of bytes read, or -1 if an error occurs or timeout is reached.
     */
    ssize_t read_with_timeout(void *buffer, const size_t length, int timeout_ms) const
    {
        pollfd pfd{.fd = sockfd, .events = POLLIN, .revents = 0};
        int ret = poll(&pfd, 1, timeout_ms);

        if (ret == -1)
        {
            // Error occurred
            return -1;
        }
        if (ret == 0)
        {
            // Timeout reached
            errno = ETIMEDOUT;
            return -1;
        }

        // Socket is ready for reading
        return ::read(sockfd, buffer, length);
    }

    /**
     * Reads data from the socket until a specific delimiter is encountered.
     * @param buffer The buffer to store the read data.
     * @param delimiter The delimiter character to stop reading at.
     * @return The number of bytes read, or -1 if an error occurs.
     */
    ssize_t read_until(std::vector<char> &buffer, const char delimiter) const
    {
        ssize_t total_bytes_read = 0;
        const size_t chunk_size  = 32;
        buffer.clear();

        while (true)
        {
            buffer.resize(buffer.size() + chunk_size);
            const ssize_t bytes_read = ::read(sockfd, buffer.data() + total_bytes_read, chunk_size);

            if (bytes_read == -1)
            {
                buffer.clear();
                return -1;
            }
            if (bytes_read == 0)
            {
                buffer.resize(total_bytes_read);
                break;
            }

            total_bytes_read += bytes_read;

            auto it = std::find(buffer.begin() + total_bytes_read - bytes_read, buffer.begin() + total_bytes_read, delimiter);
            if (it != buffer.begin() + total_bytes_read)
            {
                buffer.resize(total_bytes_read);
                break;
            }
        }

        return total_bytes_read;
    }

    /**
     * Reads data from the socket until a specific delimiter is encountered or a timeout occurs.
     * @param buffer The buffer to store the read data.
     * @param delimiter The delimiter character to stop reading at.
     * @param timeout_ms The timeout value in milliseconds.
     * @return The number of bytes read, or -1 if an error occurs or timeout is reached.
     */
    ssize_t read_until_timeout(std::vector<char> &buffer, char delimiter, int timeout_ms) const
    {
        ssize_t total_bytes_read = 0;
        const size_t chunk_size  = 32;
        buffer.clear();

        while (true)
        {
            pollfd pfd{.fd = sockfd, .events = POLLIN, .revents = 0};

            const int ret = poll(&pfd, 1, timeout_ms);

            if (ret == -1)
            {
                buffer.clear();
                return -1;
            }
            if (ret == 0)
            {
                errno = ETIMEDOUT;
                buffer.resize(total_bytes_read);
                return -1;
            }

            buffer.resize(buffer.size() + chunk_size);
            const ssize_t bytes_read = ::read(sockfd, buffer.data() + total_bytes_read, chunk_size);

            if (bytes_read == -1)
            {
                buffer.clear();
                return -1;
            }
            if (bytes_read == 0)
            {
                buffer.resize(total_bytes_read);
                break;
            }

            total_bytes_read += bytes_read;

            auto it = std::find(buffer.begin() + total_bytes_read - bytes_read, buffer.begin() + total_bytes_read, delimiter);
            if (it != buffer.begin() + total_bytes_read)
            {
                buffer.resize(total_bytes_read);
                break;
            }
        }

        return total_bytes_read;
    }

    /**
     * @brief Reads a line from the socket until a newline character ('\n') is encountered or a timeout occurs.
     *
     * This method reads data from the socket and appends it to the provided buffer until a newline character ('\n')
     * is found or the specified timeout is reached. If the buffer already contains a complete line, it extracts and
     * returns that line without reading from the socket.
     *
     * @param buffer A reference to the buffer where the data read from the socket will be stored.
     * @param timeout The maximum amount of time (in milliseconds) to wait for data to be available on the socket.
     * @param error_msg A reference to a string where an error message will be stored if the read operation fails.
     * @return The extracted line from the buffer, excluding the newline character. If an error occurs, an empty string is returned.
     */
    std::string readLine(std::vector<char> &buffer, const int timeout, std::string &error_msg) const
    {
        auto newline_pos = std::ranges::find(buffer, '\n');

        if (newline_pos == buffer.end())
        {
            ssize_t bytes_read = read_until_timeout(buffer, '\n', timeout);
            if (bytes_read == -1)
            {
                error_msg = "Failed to read line from socket, timeout or no \\n";
                return "";
            }

            newline_pos = std::ranges::find(buffer, '\n');
            if (newline_pos == buffer.end())
            {
                error_msg = "Failed to find \\n in the received data";
                return "";
            }
        }

        std::string line(buffer.begin(), newline_pos);
        buffer.erase(buffer.begin(), newline_pos + 1);
        return line;
    }

    /**
     * Sets a socket option.
     * @param level The protocol level (e.g., SOL_SOCKET).
     * @param optname The option name (e.g., SO_REUSEADDR).
     * @param optval The option value.
     * @param optlen The length of the option value.
     * @throw SocketException If setting the socket option fails.
     */
    void setSocketOption(const int level, const int optname, const void *optval, const socklen_t optlen) const
    {
        if (::setsockopt(sockfd, level, optname, optval, optlen) == -1)
        {
            throw SocketException("Failed to set socket option");
        }
    }

    /**
     * Gets a socket option.
     * @param level The protocol level (e.g., SOL_SOCKET).
     * @param optname The option name (e.g., SO_REUSEADDR).
     * @param optval The buffer to store the option value.
     * @param optlen The length of the option value buffer.
     * @throw SocketException If getting the socket option fails.
     */
    void getSocketOption(const int level, const int optname, void *optval, socklen_t *optlen) const
    {
        if (::getsockopt(sockfd, level, optname, optval, optlen) == -1)
        {
            throw SocketException("Failed to get socket option");
        }
    }

private:
    int32_t sockfd;
    sockaddr_in addr{};
};

/**
 * Connects to a server using the provided command-line arguments.
 * @param argv The command-line arguments.
 * @return The connected socket.
 */
Socket connectToServer(char *const *argv);
} // namespace zappy_gui
