#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string_view>
#define NETMESSAGE_SIZE 512

namespace MessageHandler
{
	const int InitUPDSocket(SOCKET& aSocket);
	const int InitWinSockData(WSADATA& aWinSockData);
	const int Send(const std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr);
	const int Recieve(std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr);


	std::string ParseData(std::string_view& aView, const std::string& aSeparator);
	struct Message
	{
		std::string myName;
		std::string myMessage;
	};

	Message ParseMessage(const std::string& aMessage);
}