// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <future>
#include <iostream>
#include <unordered_map>
#include <string>
#include "WinSock2.h"
#include "WS2tcpip.h"
#include "Network-Shared.h"

#define LISTEN_PORT 42000

//struct Client
//{
//	Client()
//	{
//		myAddrSize = sizeof(myAddr);
//	}
//	int myAddrSize;
//	sockaddr_in myAddr;
//};
//
//
//class Server
//{
//public:
//	Server()
//	{
//		myAddrServer = {};
//
//	};
//	const int Init()
//	{
//		std::cout << "Starting Winsock...";
//		auto result = MessageHandler::InitWinSockData(myWinSockData);
//		if (result == EXIT_FAILURE)
//			return result;
//		std::cout << "OK!" << std::endl;
//
//		result = MessageHandler::InitUPDSocket(myUDPSocket);
//		if (result == EXIT_FAILURE)
//			return result;
//
//		// Populate the server address information
//		myAddrServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc
//		myAddrServer.sin_addr.s_addr = INADDR_ANY;            // Don't care which IP address to use. Any available.
//		// Computers usually have at least two when connected
//		// to a network. One loopback and one network address.
//		// 127.0.0.1 - Built-in loopback adapter.
//		// e.g. 192.168.0.1 - Class-C address from a router.
//
//		myAddrServer.sin_port = htons(serverPort);   // The port we want to listen for connections on.
//
//		if (bind(myUDPSocket, reinterpret_cast<sockaddr*>(&myAddrServer), sizeof(myAddrServer)) == SOCKET_ERROR)
//		{
//			std::cout << "Failed to bind socket." << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//			return EXIT_FAILURE;
//		}
//
//		myInputThread = std::thread([this]
//			{
//				UpdateInput(myUDPSocket);
//			});
//
//		return EXIT_SUCCESS;
//	}
//
//	void Clear()
//	{
//		if (myInputThread.joinable())
//			myInputThread.join();
//		WSACleanup();
//	}
//
//	const bool Update()
//	{
//
//		std::string incomingData;
//		Client potentialClient;
//		const int dataLength = PollClient(potentialClient, incomingData);
//		if (dataLength == SOCKET_ERROR)
//		{
//			std::cout << "Failed receiving data from user." << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//		}
//
//		if (dataLength > 0)
//		{
//			char clientAddress[16]{ 0 };
//			inet_ntop(AF_INET, &potentialClient.myAddr.sin_addr, &clientAddress[0], 16);
//			const int clientPort = ntohs(potentialClient.myAddr.sin_port);
//
//
//
//
//			std::cout << "Packet from " << clientAddress << ":" << clientPort << std::endl;
//			std::cout << "Data: " << incomingData << std::endl;
//
//			std::string_view data = incomingData;
//
//
//			size_t pos = 0;
//			if (pos = data.find("sc_join") != std::string_view::npos && pos <= 5)
//			{
//				size_t separator = data.find("-");
//				std::string name = std::string(data.substr(separator + 1));
//				if (myCurrentClients.count(name) <= 0)
//				{
//					myCurrentClients[name] = potentialClient;
//					std::cout << name << " joined! Client Count: " << myCurrentClients.size() << "." << std::endl;
//				}
//				return myRunningFlag;
//			}
//			if (pos = data.find("sc_leave") != std::string_view::npos && pos <= 5)
//			{
//				size_t separator = data.find("-");
//				std::string name = std::string(data.substr(separator + 1));
//				if (myCurrentClients.count(name) > 0)
//				{
//					myCurrentClients.erase(name);
//					std::cout << name << " left! Client Count: " << myCurrentClients.size() << "." << std::endl;
//				}
//				return myRunningFlag;
//			}
//
//			std::string userName = "unknown";
//
//			auto msg = MessageHandler::ParseMessage(incomingData);
//
//			userName = msg.myName;
//
//
//			int commandCount = 0;
//
//			std::string_view commandCheck = msg.myMessage;
//
//			while (size_t pos = commandCheck.find("sc_") != std::string_view::npos)
//			{
//				commandCheck.remove_prefix(pos + 1);
//				commandCount++;
//			}
//
//			if (commandCount > 0)
//				msg.myMessage = "Invalid Message!";
//
//			for (auto& pair : myCurrentClients)
//			{
//				auto& client = pair.second;
//				if (pair.first == userName) continue;
//
//				if (MessageHandler::Send(incomingData, myUDPSocket, client.myAddr) == SOCKET_ERROR)
//				{
//					myRunningFlag = false;
//					return myRunningFlag;
//				}
//			}
//		}
//
//		return myRunningFlag;
//	}
//private:
//	const int PollClient(Client& aClient, std::string& someData)
//	{
//		return MessageHandler::Recieve(someData, myUDPSocket, aClient.myAddr);
//	}
//	void UpdateInput(SOCKET& aSocket)
//	{
//		// We want to be able to shut down gracefully and right now we use blocking receive
//		// so we should set up a thread to listen for keyboard input.
//		while (myRunningFlag)
//		{
//			char c;
//			std::cin.get(c);
//			if (c == 'q')
//			{
//				myRunningFlag = false;
//				// This will cause the socket operations to abort and release the socket.
//				// It will fire a SOCKET_ERROR result from recvfrom if it's presently waiting.
//				closesocket(aSocket);
//			}
//		}
//	}
//private: // Constant values
//	constexpr static uint32_t serverPort = LISTEN_PORT;
//private: //Server Data
//	bool executeCommand = false;;
//	std::thread myInputThread;
//	SOCKET myUDPSocket;
//	WSADATA myWinSockData;
//	sockaddr_in myAddrServer;
//	std::atomic<bool> myRunningFlag = true;
//private: //Client Info
//	std::unordered_map<std::string, Client> myCurrentClients;
//};



// Allows us to gracefully exit the program loop.
std::atomic<bool> isRunning = true;

int main()
{
	MessageHandler handler(HandleType::Server, SocketType::UDP);

	handler.Host(LISTEN_PORT);

	while (handler.IsHosting())
	{
		//DO SERVER STUFF HERE
	}

	/*Server server;

	if (server.Init() == EXIT_FAILURE)
	{
		server.Clear();
		return EXIT_FAILURE;
	}


	while (server.Update());
	server.Clear();*/

	return 0;
}

#pragma region OLD CODE
/*
//	// We'll need a socket to communicate on.
	//	SOCKET udpSocket;
	//	// And we'll need the Winsock data object.
	//	WSADATA winsockData;
	//
	//	// Address information we'll use to bind.
	//	sockaddr_in addrServer{};
	//
	//	// Address information of connecting client.
	//	sockaddr_in addrClient{};
	//	// We also need the size when receiving.
	//	int addrClientSize = sizeof(addrClient);
	//
	//	// Data buffer that we'll receive data into.
	//	char socketBuffer[NETMESSAGE_SIZE];
	//
	//	std::cout << "Starting Winsock...";
	//	if (WSAStartup(MAKEWORD(2, 2), &winsockData) != 0)
	//	{
	//		std::cout << " FAIL!" << std::endl;
	//		std::cout << "Error: " << WSAGetLastError() << std::endl;
	//		return EXIT_FAILURE;
	//	}
	//	std::cout << "OK!" << std::endl;
	//
	//	udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//	if (udpSocket == INVALID_SOCKET)
	//	{
	//		std::cout << "Failed to create socket." << std::endl;
	//		std::cout << "Error: " << WSAGetLastError() << std::endl;
	//		return EXIT_FAILURE;
	//	}
	//
	//	// Populate the server address information
	//	addrServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc
	//	addrServer.sin_addr.s_addr = INADDR_ANY;            // Don't care which IP address to use. Any available.
	//	// Computers usually have at least two when connected
	//	// to a network. One loopback and one network address.
	//	// 127.0.0.1 - Built-in loopback adapter.
	//	// e.g. 192.168.0.1 - Class-C address from a router.
	//
	//	addrServer.sin_port = htons(LISTEN_PORT);   // The port we want to listen for connections on.
	//	// htons converts the byte order from host order to
	//	// network order.
	//
	//// We need to bind to a specific address and port combination. This tells the
	//// operating system that we want communication on that combination to go to
	//// this program.
	//	if (bind(udpSocket, reinterpret_cast<sockaddr*>(&addrServer), sizeof addrServer) == SOCKET_ERROR)
	//	{
	//		std::cout << "Failed to bind socket." << std::endl;
	//		std::cout << "Error: " << WSAGetLastError() << std::endl;
	//		return EXIT_FAILURE;
	//	}
	//
	//
	//	// If we got this far we should now have an open socket ready to receive information from the network.
	//	while (isRunning)
	//	{
	//		// Clear the buffer.
	//		ZeroMemory(socketBuffer, NETMESSAGE_SIZE);
	//
	//		// blocking receive. This function will block until a message is received.
	//		const int recv_len = recvfrom(udpSocket, socketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&addrClient, &addrClientSize);
	//		if (recv_len == SOCKET_ERROR)
	//		{
	//			std::cout << "Failed receiving data from socket." << std::endl;
	//			std::cout << "Error: " << WSAGetLastError() << std::endl;
	//		}
	//
	//		if (recv_len > 0)
	//		{
	//			// Extract the address information from the incoming data.
	//			// 16 bytes is enough for an IPv4 address.
	//			// i.e. "xxx.xxx.xxx.xxx" + string terminator
	//			char clientAddress[16]{ 0 };
	//			inet_ntop(AF_INET, &addrClient.sin_addr, &clientAddress[0], 16);
	//			const int clientPort = ntohs(addrClient.sin_port);
	//
	//			std::cout << "Packet from " << clientAddress << ":" << clientPort << std::endl;
	//			std::cout << "Data: " << socketBuffer << std::endl;
	//
	//			// Send it back
	//			if (sendto(udpSocket, socketBuffer, NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr*>(&addrClient), sizeof(addrClient)) == SOCKET_ERROR)
	//			{
	//				std::cout << "Error: " << WSAGetLastError() << std::endl;
	//				isRunning = false;
	//				break;
	//			}
	//		}
	//	}
	//
	//	inputThread.join();
	//	WSACleanup();


*/
#pragma endregion