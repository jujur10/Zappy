#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <cstdint>
#include <string>
#include <vector>
#include <exception>

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
    explicit SocketException(std::string message);

    /**
     * Returns the error message string.
     * @return The error message as a C-style string.
     */
    [[nodiscard]] const char *what() const noexcept override;

   private:
    std::string errorMessage;
};

/**
 * @class Socket
 * Socket class for TCP/IP communication.
 *
 * This class provides functionality for creating a socket, connecting to a
 * server, reading from and writing to the socket, and setting/getting socket
 * options.
 */
class Socket
{
   public:
    /**
     * Constructor for Socket.
     * @param port The port number to bind the socket to.
     * @throw SocketException If socket creation fails.
     */
    explicit Socket(uint16_t port);

    /**
     * @brief Destructor for Socket.
     *
     * Closes the socket.
     */
    ~Socket();

    /**
     * @brief Socket file descriptor getter.
     * @return The file descriptor of the socket.
     */
    [[nodiscard]] int32_t GetSocketFd() const;

    /**
     * @brief Socket address getter.
     * @return The address of the socket.
     */
    [[nodiscard]] sockaddr_in GetAddress() const;

    /**
     * @brief Binds the socket to an address.
     *
     * This method binds the socket to the address specified in the constructor.
     *
     * @throw SocketException If binding the socket fails.
     */
    void Bind() const;

    /**
     * @brief Listens for incoming connections on the socket.
     * @param connectionNumber The maximum number of connections to queue.
     * @return true if listening is successful, false otherwise.
     */
    [[nodiscard]] bool Listen(int32_t connectionNumber) const;

    /**
     * @brief Accepts a connection on the socket.
     * @param clientAddress The address of the client.
     * @param clientAddressLength The length of the client address.
     * @return The file descriptor of the client socket.
     */
    int32_t Accept(sockaddr_in *clientAddress, socklen_t *clientAddressLength) const;

    /**
     * Connects the socket to a server.
     * @param ip The IP address of the server.
     * @param port The port number of the server.
     * @return true if the connection is successful, false otherwise.
     * @throw SocketException If the IP address is invalid or connection fails.
     */
    bool Connect(const char *ip, uint16_t port);

    /**
     * Writes data to the socket.
     * @param buffer The buffer containing the data to write.
     * @param length The length of the data to write.
     * @return The number of bytes written, or -1 if an error occurs.
     */
    ssize_t Write(const char *buffer, size_t length) const;

    /**
     * Reads data from the socket.
     * @param buffer The buffer to store the read data.
     * @param length The maximum length of data to read.
     * @return The number of bytes read, or -1 if an error occurs.
     */
    ssize_t Read(char *buffer, size_t length) const;

    /**
     * Reads data from the socket with a timeout.
     * @param buffer The buffer to store the read data.
     * @param length The maximum length of data to read.
     * @param timeoutMs The timeout value in milliseconds.
     * @return The number of bytes read, or -1 if an error occurs or timeout is
     * reached.
     */
    ssize_t ReadWithTimeout(char *buffer, size_t length, int timeoutMs) const;

    /**
     * Reads data from the socket until a specific delimiter is encountered. You
     * are responsible for clearing the buffer after processing the line.
     * @param buffer The buffer to store the read data.
     * @param delimiter The delimiter character to stop reading at.
     * @return The number of bytes read, or -1 if an error occurs.
     */
    ssize_t ReadUntil(std::vector<char> &buffer, char delimiter) const;

    /**
     * Reads data from the socket until a specific delimiter is encountered or a
     * timeout occurs. You are responsible for clearing the buffer after
     * processing the line.
     * @param buffer The buffer to store the read data.
     * @param delimiter The delimiter character to stop reading at.
     * @param timeoutMs The timeout value in milliseconds.
     * @return The number of bytes read, or -1 if an error occurs or timeout is
     * reached.
     */
    ssize_t ReadUntilTimeout(std::vector<char> &buffer, char delimiter, int timeoutMs) const;

    /**
     * Reads data from the socket until a specific delimiter is encountered.
     * Using non blocking recv.
     * You are responsible for clearing the buffer after processing the line.
     * @param buffer The buffer to store the read data.
     * @param delimiter The delimiter character to stop reading at.
     * @return The number of bytes read, or -1 if an error occurs.
     */
    ssize_t ReadUntilFast(std::vector<char> &buffer, char delimiter) const;

    /**
     * @brief Reads a line from the socket until a newline character ('\n') is
     * encountered or a timeout occurs.
     *
     * This method reads data from the socket and appends it to the provided
     * buffer until a newline character ('\n') is found or the specified timeout
     * is reached. If the buffer already contains a complete line, it extracts
     * and returns that line without reading from the socket.
     *
     * @param buffer A reference to the buffer where the data read from the
     * socket will be stored.
     * @param timeout The maximum amount of time (in milliseconds) to wait for
     * data to be available on the socket.
     * @param errorMsg A reference to a string where an error message will be
     * stored if the read operation fails.
     * @return The extracted line from the buffer, excluding the newline
     * character. If an error occurs, an empty string is returned.
     */
    std::string ReadLineTimeout(std::vector<char> &buffer, int timeout, std::string &errorMsg) const;

    /**
     * @brief Reads a line from the socket until a newline character ('\n') is
     * encountered or a timeout occurs.
     *
     * This method reads data from the socket and appends it to the provided
     * buffer until a newline character ('\n') is found or the specified timeout
     * is reached. If the buffer already contains a complete line, it extracts
     * and returns that line without reading from the socket.
     *
     * @param buffer A reference to the buffer where the data read from the
     * socket will be stored.
     * @param errorMsg A reference to a string where an error message will be
     * stored if the read operation fails.
     * @return The extracted line from the buffer, excluding the newline
     * character. If an error occurs, an empty string is returned.
     */
    std::string ReadLine(std::vector<char> &buffer, std::string &errorMsg) const;

    /**
     * @brief Reads a line using recv from the socket until a newline character
     * ('\n') is encountered or no more data is available.
     *
     * This method reads data from the socket and appends it to the provided
     * buffer until a newline character ('\n') is found or the specified timeout
     * is reached. If the buffer already contains a complete line, it extracts
     * and returns that line without reading from the socket.
     *
     * @param buffer A reference to the buffer where the data read from the
     * socket will be stored.
     * @return The extracted line from the buffer, excluding the newline
     * character. If an error occurs, an empty string is returned.
     */
    std::string ReadLineFast(std::vector<char> &buffer) const;

    /**
     * Sets a socket option.
     * @param level The protocol level (e.g., SOL_SOCKET).
     * @param optname The option name (e.g., SO_REUSEADDR).
     * @param optval The option value.
     * @param optlen The length of the option value.
     * @throw SocketException If setting the socket option fails.
     */
    void SetSocketOption(int level, int optname, const void *optval, socklen_t optlen) const;

    /**
     * Gets a socket option.
     * @param level The protocol level (e.g., SOL_SOCKET).
     * @param optname The option name (e.g., SO_REUSEADDR).
     * @param optval The buffer to store the option value.
     * @param optlen The length of the option value buffer.
     * @throw SocketException If getting the socket option fails.
     */
    void GetSocketOption(int level, int optname, void *optval, socklen_t *optlen) const;

   private:
    int32_t _sockFd;
    sockaddr_in _addr{};
};

/**
 * Connects to a server using the provided command-line arguments.
 * @param argv The command-line arguments.
 * @return The connected socket.
 */
Socket ConnectToServer(char *const *argv);
}  // namespace zappy_gui
