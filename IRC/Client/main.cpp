#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

const int BUFFER_SIZE = 4096;
const std::string SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

void receiveMessages(SOCKET serverSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytesRead = recv(serverSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead > 0) {
            std::cout << std::string(buffer, bytesRead) << std::endl;
        }
        else if (bytesRead == 0) {
            std::cout << "Disconnected from the server." << std::endl;
            break;
        }
        else {
            std::cout << "Error in receiving message from the server." << std::endl;
            break;
        }
    }
}

int main() {
    // Initialize Winsock for Windows
#ifdef _WIN32
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cout << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
#endif

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket." << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Set up server details
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cout << "Invalid address or address not supported." << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
        WSACleanup();
#else
        close(clientSocket);
#endif
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "Failed to connect to the server." << std::endl;
#ifdef _WIN32
        closesocket(clientSocket);
        WSACleanup();
#else
        close(clientSocket);
#endif
        return 1;
    }

    // Create a separate thread to receive messages from the server
    std::thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    std::cout << "Connected to the server." << std::endl;

    // Main thread for handling user input and sending messages
    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }
        else if (message.empty()) {
            continue;
        }

        int bytesSent = send(clientSocket, message.c_str(), message.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Failed to send message to the server." << std::endl;
            break;
        }
    }

    // Clean up
#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif

    return 0;
}
