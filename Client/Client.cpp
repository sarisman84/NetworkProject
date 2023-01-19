// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <string>

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "Network-Shared.h"

#define SERVER_ADDRESS L"127.0.0.1"
#define SERVER_PORT 42000

// Allows us to gracefully exit the program loop.
//std::atomic<bool> isRunning = true;

//class Client
//{
//public:
//	Client() = default;
//	const int Init()
//	{
//		std::cout << "Starting Winsock...";
//		auto result = InitWinSock();
//		if (result == EXIT_FAILURE)
//			return result;
//		std::cout << "OK!" << std::endl;
//		result = InitSocket();
//		if (result == EXIT_FAILURE)
//			return result;
//
//		// Tell the system where the server is that we want to talk with.
//		myAddrServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc
//
//		// Unlike on the Server we cannot specify INADDR_ANY since we need to know
//		// which destination IP address to use.
//		InetPton(AF_INET, TEXT(SERVER_ADDRESS), &myAddrServer.sin_addr.s_addr);
//
//		myAddrServer.sin_port = htons(SERVER_PORT);   // The port that the server is listening on
//		// htons converts the byte order from host order to
//		// network order.
//
//		// Why no bind on client? We leave that to the operating system!
//		// By calling sendto we cause Windows to implicitly bind the udpSocket
//		// by filling in all the information and giving us a random local port.
//		// This then allows us to use recvfrom as we please because the udpSocket
//		// is already bound for us in an appropriate way.
//
//		myConsoleInput = std::thread([this]()
//			{
//				while (myRunningFlag)
//				{
//					while (myInputCheck)
//						std::this_thread::yield();
//					std::cin.getline(myMessage, messageSize);
//					myInputCheck = true;
//				}
//			});
//
//
//		std::cout << "Enter Username: ";
//		bool assignedUserName = false;
//		while (!assignedUserName)
//		{
//			std::cin.getline(myMessage, messageSize);
//			myName = myMessage;
//			if (!myName.empty())
//				assignedUserName = true;
//		}
//
//		Send(std::string("sc_join-") + myName);
//		std::cout << "Welcome! (write quit to quit) ";
//
//		return EXIT_SUCCESS;
//	}
//
//	const bool Update()
//	{
//		if (myInputCheck)
//		{
//			if (strcmp(myMessage, "quit") == 0)
//			{
//				return false;
//			}
//
//			std::string finalMessage = std::string("sc_n-") + myName + ":" + myMessage;
//			auto result = Send(finalMessage);
//			if (result == EXIT_FAILURE)
//			{
//				return false;
//			}
//
//			myInputCheck = false;
//		}
//
//		std::string incomingMessage;
//		const int recv_len = Recieve(incomingMessage);
//
//		if (recv_len == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
//		{
//			std::cout << "Failed receiving data from udpSocket." << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//		}
//
//		if (recv_len > 0)
//		{
//
//			auto msg = MessageHandler::ParseMessage(incomingMessage);
//			std::cout << msg.myName << ": " << msg.myMessage << std::endl;
//		}
//
//		return myRunningFlag;
//	}
//
//	const int OnDisconnect()
//	{
//		auto result = Send(std::string("sc_leave-") + myName);
//		return result;
//	}
//
//
//	void Clear()
//	{
//		if (myConsoleInput.joinable())
//			myConsoleInput.join();
//		closesocket(myUDPSocket);
//		WSACleanup();
//	}
//private:
//	const int InitWinSock() {
//
//		if (WSAStartup(MAKEWORD(2, 2), &myWinSockData) != 0)
//		{
//			std::cout << " FAIL!" << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//			return EXIT_FAILURE;
//		}
//		return EXIT_SUCCESS;
//	}
//	const int InitSocket()
//	{
//		myUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//		if (myUDPSocket == INVALID_SOCKET)
//		{
//			std::cout << "Failed to create udpSocket." << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//			return EXIT_FAILURE;
//		}
//		// Set udpSocket to non blocking
//		// This way recvfrom will not wait until it gets a message
//		u_long iMode = true;
//		ioctlsocket(myUDPSocket, FIONBIO, &iMode);
//
//		return EXIT_SUCCESS;
//	}
//	const int Send(const std::string& someData)
//	{
//		return MessageHandler::Send(someData, myUDPSocket, myAddrServer);
//	}
//	const int Recieve(std::string& someData)
//	{
//		return MessageHandler::Recieve(someData, myUDPSocket, myAddrServer);
//	}
//private:
//	static constexpr int serverPort = SERVER_PORT;
//	static constexpr char serverAdress[16] = SERVER_ADDRESS;
//	static constexpr int messageSize = 512;
//private: //Client data
//	std::string myName;
//	char myMessage[messageSize];
//	SOCKET myUDPSocket;
//	WSADATA myWinSockData;
//	sockaddr_in myAddrServer;
//private: //Thread variables
//	std::thread myConsoleInput;
//	std::atomic<bool> myRunningFlag = true;
//	std::atomic<bool> myInputCheck;
//};


int main()
{
	/*Client client;
	auto result = client.Init();
	if (result == EXIT_FAILURE)
	{
		client.Clear();
		return result;
	}

	while (client.Update());

	result = client.OnDisconnect();
	if (result == EXIT_FAILURE)
	{
		client.Clear();
		return result;
	}

	client.Clear();*/


	MessageHandler handler(HandleType::Client,SocketType::UDP);
	UserInfo info{
		EventType::Join,
		0,
		"Yes"
	};
	while (handler.TryConnect(SERVER_ADDRESS, SERVER_PORT, info,1000));
	while (handler.IsConnected());

	return 0;
}