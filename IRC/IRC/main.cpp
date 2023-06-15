#include <vector>
#include <unordered_map>
#include <thread>
#include <algorithm>
#include <iostream>
#include <string>

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


#define PORT 8080
#define CMD_PREFIX '/'

struct Client {
	std::string nickname;
	std::string currentRoom;
};

struct Room {
	std::string owner;
	std::vector<SOCKET> clients;
};

std::unordered_map<SOCKET, Client> clients;
std::unordered_map<std::string, Room> rooms;

void sendToClient(SOCKET clientSocket, const std::string& message) {
	send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);
}

void broadcastToRoom(const std::string& message, const std::string& room, SOCKET senderSocket) {
	if (rooms.find(room) != rooms.end()) {
		for (const auto& socket : rooms[room].clients) {
			if (socket != senderSocket) {
				sendToClient(socket, message);
			}
		}
	}
}

void handleCommand(const std::string& message, SOCKET clientSocket) {
	std::string response;

	// Check if the message is a command
	if (!message.empty() && message[0] == CMD_PREFIX) {
		// Parse the command and perform the corresponding actions
		std::string command = message.substr(1);

		if (command.substr(0, 5) == "login") {
			std::string nickname;
			if (command.length() > 6) {
				nickname = command.substr(6);
			}

			if (clients[clientSocket].nickname.size()) {
				response = "You are already logged in, you must logout first to login to an another account";
			}
			else if (nickname.empty()) {
				response = "Invalid nickname. Please enter a valid nickname.";
			}
			else {
				// Check if nickname is already in use
				auto it = std::find_if(clients.begin(), clients.end(),
					[&](const auto& client) { return client.second.nickname == nickname; });
				if (it != clients.end()) {
					response = "Nickname is already in use. Please choose a different nickname.";
				}
				else {
					Client newClient;
					newClient.nickname = nickname;
					newClient.currentRoom = "lobby";
					clients[clientSocket] = newClient;

					rooms[newClient.currentRoom].clients.push_back(clientSocket);

					broadcastToRoom(nickname + " entered the lobby.\n", "lobby", clientSocket);

					response = "Logged in as " + nickname;
				}
			}
		}
		else if (command == "logout") {
			auto it = clients.find(clientSocket);
			if (it != clients.end()) {
				std::string nickname = it->second.nickname;
				std::string currentRoom = it->second.currentRoom;

				broadcastToRoom(nickname + " left the room.\n", currentRoom, clientSocket);

				// Remove the client from the chat room
				if (rooms.find(currentRoom) != rooms.end()) {
					auto socketIt = std::find(rooms[currentRoom].clients.begin(), rooms[currentRoom].clients.end(), clientSocket);
					if (socketIt != rooms[currentRoom].clients.end()) {
						rooms[currentRoom].clients.erase(socketIt);
					}
				}

				// Remove the client from the client list
				clients.erase(it);

				response = "Logged out: " + nickname;
			}
			else {
				response = "Not logged in.";
			}
		}
		else if (command == "list rooms") {
			response = "Available chat rooms:\n";
			for (const auto& room : rooms) {
				response += room.first + "\n";
			}
		}
		else if (command == "list users") {
			auto it = clients.find(clientSocket);
			if (it != clients.end()) {
				std::string room = it->second.currentRoom;
				if (rooms.find(room) != rooms.end()) {
					response = "Users in room " + room + ":\n";
					for (const auto& socket : rooms[room].clients) {
						auto clientIt = clients.find(socket);
						if (clientIt != clients.end()) {
							response += clientIt->second.nickname + "\n";
						}
					}
				}
				else {
					response = "You are not in any room.";
				}
			}
			else {
				response = "Not logged in.";
			}
		}
		else if (command == "whereami") {
			auto it = clients.find(clientSocket);
			if (it != clients.end()) {
				std::string room = it->second.currentRoom;
				response = "You are in room: " + room;
			}
			else {
				response = "Not logged in.";
			}
		}
		else if (command.substr(0, 5) == "enter") {
			std::string room;
			if (command.length() > 6) {
				room = command.substr(6);
			}

			if (room.empty()) {
				response = "Invalid room name. Please enter a valid room name.";
			}
			else {
				auto it = clients.find(clientSocket);
				if (it != clients.end()) {
					std::string currentRoom = it->second.currentRoom;

					if (rooms.find(currentRoom) != rooms.end()) {
						auto socketIt = std::find(rooms[currentRoom].clients.begin(), rooms[currentRoom].clients.end(), clientSocket);
						if (socketIt != rooms[currentRoom].clients.end()) {
							rooms[currentRoom].clients.erase(socketIt);
						}
					}

					broadcastToRoom(it->second.nickname + " left the room.\n", currentRoom, clientSocket);

					if (rooms.find(room) != rooms.end()) {
						rooms[room].clients.push_back(clientSocket);
					}
					else {
						Room newRoom;
						newRoom.owner = it->second.nickname;
						newRoom.clients = { clientSocket };
						rooms[room] = newRoom;
					}

					it->second.currentRoom = room;
					response = "Entered room: " + room;

					broadcastToRoom(it->second.nickname + " entered the room.\n", room, clientSocket);
				}
				else {
					response = "Not logged in.";
				}
			}
		}
		else if (command.substr(0, 5) == "close") {
			std::string room;
			if (command.length() > 6) {
				room = command.substr(6);
			}

			if (room.empty()) {
				response = "Invalid room name. Please enter a valid room name.";
			}
			else {
				auto it = clients.find(clientSocket);
				if (it != clients.end()) {
					std::string nickname = it->second.nickname;

					if (rooms.find(room) != rooms.end() && nickname == rooms[room].owner) {
						broadcastToRoom("Room is closed, you are moved to the lobby.", room, clientSocket);
						// Move clients to the lobby

						for (const auto& socket : rooms[room].clients) {
							clients[socket].currentRoom = "lobby";
							rooms["lobby"].clients.push_back(socket);
						}

						rooms.erase(room);

						response = "Room is closed";
					}
					else {
						response = "You are not the owner of the room.";
					}
				}
				else {
					response = "Not logged in.";
				}
			}
		}
		else if (command.substr(0, 4) == "open") {
			std::string room;
			if (command.length() > 5) {
				room = command.substr(5);
			}

			if (room.empty()) {
				response = "Invalid room name. Please enter a valid room name.";
			}
			else {
				if (rooms.find(room) != rooms.end()) {
					response = "Room already exists.";
				}
				else {
					Room newRoom;
					newRoom.owner = clients[clientSocket].nickname;
					newRoom.clients = {};
					rooms[room] = newRoom;
					response = "Created room: " + room;
				}
			}
		}
		else {
			response = "Invalid command. Available commands:\n";
			response += "login <nickname>: Registers a user with the given nickname on the IRC server.\n";
			response += "logout: Closes the user's session on the server.\n";
			response += "list rooms: Lists the current chat rooms on the server.\n";
			response += "list users: Lists the users in the current chat room.\n";
			response += "enter <room_name>: Allows a guest to enter (connect to) a chat room.\n";
			response += "close <room_name>: Closes the specified chat room. Only the room's creator can use this command.\n";
			response += "open <room_name>: Creates a new chat room.";
		}
	}
	else {
		auto it = clients.find(clientSocket);
		if (it != clients.end()) {
			std::string room = it->second.currentRoom;
			if (rooms.find(room) != rooms.end()) {
				std::string nickname = it->second.nickname;
				response = "[" + room + "] " + nickname + ": " + message;
				broadcastToRoom(response, room, clientSocket);
			}
			else {
				response = "You are not in any room.";
			}
		}
		else {
			response = "Not logged in.";
		}
	}

	sendToClient(clientSocket, response + "\n");
}

void handleClient(SOCKET clientSocket) {
	char buffer[1024] = { 0 };
	std::string message;

	while (true) {
		memset(buffer, 0, sizeof(buffer));
		int valread = recv(clientSocket, buffer, 1024, 0);

		if (valread <= 0) {
			break;
		}

		std::string command(buffer);

		handleCommand(command, clientSocket);
	}

	auto it = clients.find(clientSocket);
	if (it != clients.end()) {
		std::string nickname = it->second.nickname;

		std::string currentRoom = it->second.currentRoom;
		if (rooms.find(currentRoom) != rooms.end()) {
			auto socketIt = std::find(rooms[currentRoom].clients.begin(), rooms[currentRoom].clients.end(), clientSocket);
			if (socketIt != rooms[currentRoom].clients.end()) {
				rooms[currentRoom].clients.erase(socketIt);
			}
		}

		clients.erase(it);

		std::cout << "Client disconnected: " << nickname << std::endl;
	}

	closesocket(clientSocket);
}

int main() {
	// Initialize Winsock on Windows
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock" << std::endl;
		return 1;
	}
#endif

	// Create a server socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket" << std::endl;
		return 1;
	}

	// Bind the socket to a specific IP and port
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
		std::cerr << "Failed to bind socket" << std::endl;
#ifdef _WIN32
		closesocket(serverSocket);
#else
		close(serverSocket);
#endif
		return 1;
	}

	// Start listening for client connections
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Failed to listen" << std::endl;
#ifdef _WIN32
		closesocket(serverSocket);
#else
		close(serverSocket);
#endif
		return 1;
	}

	// Accept client connections and handle them in separate threads
	while (true) {
		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Failed to accept client connection" << std::endl;
#ifdef _WIN32
			closesocket(serverSocket);
#else
			close(serverSocket);
#endif
			return 1;
		}

		// Create a new thread to handle the client connection
		std::thread clientThread(handleClient, clientSocket);
		clientThread.detach();
	}

	// Clean up Winsock on Windows
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}
