//
// Created by quentinf on 08/06/24.
//
#include <criterion/criterion.h>
#include "sockets.hpp"
#include <thread>
#include <chrono>

using namespace zappy_gui;

void startTestServer(uint16_t port, const std::string& response)
{
    Socket serverSocket(port);

    // Set socket options
    int optval = 1;
    serverSocket.SetSocketOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Bind the socket to the specified port
    serverSocket.Bind();

    // Listen for incoming connections
    if (!serverSocket.Listen(1))
    {
        throw SocketException("Failed to listen on socket");
    }

    // Accept a client connection
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    const int clientSocketFd = serverSocket.Accept(&clientAddress, &clientAddressLength);
    if (clientSocketFd == -1)
    {
        throw SocketException("Failed to accept client connection");
    }

    // Send the response to the client
    if (write(clientSocketFd, response.c_str(), response.length()) == -1)
    {
        throw SocketException("Failed to write response to client");
    }

    // Close the client socket
    close(clientSocketFd);
}

Test(Socket_Constructor, ValidPort)
{
    uint16_t port = 8080;
    Socket socket(port);
    cr_assert(true, "Socket constructor should not throw an exception");
}

Test(Socket_Connect, InvalidIP)
{
    Socket socket(8080);
    const char* ip = "invalid_ip";
    uint16_t port = 8080;
    cr_assert_throw(socket.Connect(ip, port), SocketException, "Socket should throw an exception for an invalid IP");
}

Test(Socket_Write, ValidData)
{
    uint16_t port = 8080;
    std::string response = "Hello, client!";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    const char* data = "Hello, server!";
    ssize_t length = strlen(data);
    ssize_t bytesWritten = socket.Write(data, length);
    cr_assert_eq(bytesWritten, length, "Socket should write the correct number of bytes");

    serverThread.join();
}

Test(Socket_Read, ValidData)
{
    uint16_t port = 8081;
    std::string response = "Response from server";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    char buffer[256] = {0};
    ssize_t bytesRead = socket.Read(buffer, sizeof(buffer));
    cr_assert_eq(bytesRead, response.length(), "Socket should read the correct number of bytes");
    cr_assert_str_eq(buffer, response.c_str(), "Socket should read the expected response");

    serverThread.join();
}


Test(Socket_ReadWithTimeout, ValidData)
{
    uint16_t port = 8082;
    std::string response = "Response from server";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    char buffer[256] = {0};
    int timeoutMs = 1000;
    ssize_t bytesRead = socket.ReadWithTimeout(buffer, sizeof(buffer), timeoutMs);
    cr_assert_eq(bytesRead, response.length(), "Socket should read the correct number of bytes within the timeout");
    cr_assert_str_eq(buffer, response.c_str(), "Socket should read the expected response");

    serverThread.join();
}

Test(Socket_ReadUntil, ValidDelimiter)
{
    uint16_t port = 8083;
    std::string response = "Data with delimiter\n";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    std::vector<char> buffer;
    char delimiter = '\n';
    ssize_t bytesRead = socket.ReadUntil(buffer, delimiter);
    cr_assert_eq(bytesRead, response.length(), "Socket should read the correct number of bytes until the delimiter");
    cr_assert_eq(buffer.back(), delimiter, "The last character in the buffer should be the delimiter");

    serverThread.join();
}

Test(Socket_ReadUntilTimeout, ValidDelimiter)
{
    uint16_t port = 8084;
    std::string response = "Data with delimiter\n";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    std::vector<char> buffer;
    char delimiter = '\n';
    int timeoutMs = 1000;
    ssize_t bytesRead = socket.ReadUntilTimeout(buffer, delimiter, timeoutMs);
    cr_assert_eq(bytesRead, response.length(), "Socket should read the correct number of bytes until the delimiter within the timeout");
    cr_assert_eq(buffer.back(), delimiter, "The last character in the buffer should be the delimiter");

    serverThread.join();
}

Test(Socket_ReadLine, ValidLine)
{
    uint16_t port = 8085;
    std::string response = "Line of text\n";
    std::thread serverThread(startTestServer, port, response);

    // Give the server some time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Socket socket(port);
    socket.Connect("127.0.0.1", port);

    std::vector<char> buffer;
    int timeoutMs = 1000;
    std::string errorMsg;
    std::string line = socket.ReadLineTimeout(buffer, timeoutMs, errorMsg);
    cr_assert_eq(line, response.substr(0, response.length() - 1), "Socket should read the expected line");
    cr_assert_str_empty(errorMsg.c_str(), "No error message should be set");

    serverThread.join();
}

Test(Socket_SetSocketOption, ValidOption)
{
    Socket socket(8080);
    int optval = 1;
    socket.SetSocketOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    cr_assert(true, "Setting a valid socket option should not throw an exception");
}

Test(Socket_GetSocketOption, ValidOption)
{
    Socket socket(8080);
    int optval;
    socklen_t optlen = sizeof(optval);
    socket.GetSocketOption(SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
    cr_assert_eq(optlen, sizeof(optval), "Getting a valid socket option should set the correct option length");
}