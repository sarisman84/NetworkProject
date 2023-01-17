#include "pch.h"
#include "Network-Shared.h"
#include <iostream>
namespace MessageHandler
{
	const int InitUPDSocket(SOCKET& aSocket)
	{
		aSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (aSocket == INVALID_SOCKET)
		{
			std::cout << "Failed to create socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	const int InitWinSockData(WSADATA& aWinSockData)
	{
		if (WSAStartup(MAKEWORD(2, 2), &aWinSockData) != 0)
		{
			std::cout << " FAIL!" << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	const int Send(const std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr)
	{
		if (sendto(aSocket, someData.c_str(), someData.length(), 0, reinterpret_cast<sockaddr*>(&anAddr), sizeof(anAddr)) == SOCKET_ERROR)
		{
			std::string readableErrorMSG = std::to_string(WSAGetLastError());
			auto error = WSAGetLastError();
			if (error == WSAEAFNOSUPPORT)
				readableErrorMSG = std::string("Protocol Family on incoming socket not supported! [") + std::to_string(WSAGetLastError()) + "]";
			

			std::cout << "Error: " << readableErrorMSG << ". [Data: " << someData << "]" << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	const int Recieve(std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr)
	{
		char socketBuffer[NETMESSAGE_SIZE];
		ZeroMemory(socketBuffer, NETMESSAGE_SIZE);
		int addrServerSize = sizeof(anAddr);
		const int recv_len = recvfrom(aSocket, socketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&anAddr, &addrServerSize);
		someData = std::string(socketBuffer);
		return recv_len;
	}


	std::string ParseData(std::string_view& aView, const std::string& aSeparator)
	{
		auto separator = aView.find(aSeparator);
		if (separator != std::string_view::npos)
		{
			aView.remove_prefix(separator + 1);
			return std::string(aView);
		}
		return std::string();
	}

	Message ParseMessage(const std::string& aMessage)
	{
		std::string_view sv = aMessage;
		Message msg;
		// get the word after the dash
		auto dash_pos = sv.find("-");
		if (dash_pos != std::string_view::npos)
		{
			auto word_after_dash = sv.substr(dash_pos + 1);
			auto colon_pos = word_after_dash.find(":");
			if (colon_pos != std::string_view::npos)
			{
				msg.myName = word_after_dash.substr(0, colon_pos);
			}
		}

		// get the rest of the string after the colon
		auto colon_pos = sv.find(":");
		if (colon_pos != std::string_view::npos)
		{
			auto rest_of_string = sv.substr(colon_pos + 1);
			msg.myMessage = rest_of_string;
		}
	
		return msg;
	}
}