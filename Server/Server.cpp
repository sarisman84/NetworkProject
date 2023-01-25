// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <future>
#include <iostream>
#include <unordered_map>
#include <string>
#include "WinSock2.h"
#include "WS2tcpip.h"
#include "Network-Shared.h"
#include "NetworkServer.h"
#define LISTEN_PORT 42000

#include <Windows.h>


NetworkServer server(LISTEN_PORT);

BOOL WINAPI ConsoleEvents(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		server.OnShutdown();
		return TRUE;
	default:
		return FALSE;
	}
}

int main()
{
	SetConsoleCtrlHandler(ConsoleEvents, TRUE);
	while (server.Update());
	server.OnShutdown();
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