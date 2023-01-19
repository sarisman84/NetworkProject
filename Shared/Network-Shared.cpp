#include "pch.h"
#include "Network-Shared.h"
#include <iostream>
#include <future>
#include <chrono>
//namespace MessageHandler
//{
//	const int InitUPDSocket(SOCKET& aSocket)
//	{
//		aSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//		if (aSocket == INVALID_SOCKET)
//		{
//			std::cout << "Failed to create socket." << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//			return EXIT_FAILURE;
//		}
//		return EXIT_SUCCESS;
//	}
//
//	const int InitWinSockData(WSADATA& aWinSockData)
//	{
//		if (WSAStartup(MAKEWORD(2, 2), &aWinSockData) != 0)
//		{
//			std::cout << " FAIL!" << std::endl;
//			std::cout << "Error: " << WSAGetLastError() << std::endl;
//			return EXIT_FAILURE;
//		}
//		return EXIT_SUCCESS;
//	}
//
//	const int Send(const std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr)
//	{
//		if (sendto(aSocket, someData.c_str(), someData.length(), 0, reinterpret_cast<sockaddr*>(&anAddr), sizeof(anAddr)) == SOCKET_ERROR)
//		{
//			std::string readableErrorMSG = std::to_string(WSAGetLastError());
//			auto error = WSAGetLastError();
//			if (error == WSAEAFNOSUPPORT)
//				readableErrorMSG = std::string("Protocol Family on incoming socket not supported! [") + std::to_string(WSAGetLastError()) + "]";
//			
//
//			std::cout << "Error: " << readableErrorMSG << ". [Data: " << someData << "]" << std::endl;
//			return EXIT_FAILURE;
//		}
//		return EXIT_SUCCESS;
//	}
//
//	const int Recieve(std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr)
//	{
//		char socketBuffer[NETMESSAGE_SIZE];
//		ZeroMemory(socketBuffer, NETMESSAGE_SIZE);
//		int addrServerSize = sizeof(anAddr);
//		const int recv_len = recvfrom(aSocket, socketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&anAddr, &addrServerSize);
//		someData = std::string(socketBuffer);
//		return recv_len;
//	}
//
//
//	std::string ParseData(std::string_view& aView, const std::string& aSeparator)
//	{
//		auto separator = aView.find(aSeparator);
//		if (separator != std::string_view::npos)
//		{
//			aView.remove_prefix(separator + 1);
//			return std::string(aView);
//		}
//		return std::string();
//	}
//
//	Message ParseMessage(const std::string& aMessage)
//	{
//		std::string_view sv = aMessage;
//		Message msg;
//		// get the word after the dash
//		auto dash_pos = sv.find("-");
//		if (dash_pos != std::string_view::npos)
//		{
//			auto word_after_dash = sv.substr(dash_pos + 1);
//			auto colon_pos = word_after_dash.find(":");
//			if (colon_pos != std::string_view::npos)
//			{
//				msg.myName = word_after_dash.substr(0, colon_pos);
//			}
//		}
//
//		// get the rest of the string after the colon
//		auto colon_pos = sv.find(":");
//		if (colon_pos != std::string_view::npos)
//		{
//			auto rest_of_string = sv.substr(colon_pos + 1);
//			msg.myMessage = rest_of_string;
//		}
//	
//		return msg;
//	}
//}

MessageHandler::MessageHandler(const HandleType aHandleType, const SocketType aSocketType)
{
	std::cout << "Initializing Message Handler ... ";
	if (WSAStartup(MAKEWORD(2, 2), &myWinSockContext) != 0)
	{
		std::cout << " Failed to initialize winsock2!" << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return;
	}
	switch (aSocketType)
	{
	case SocketType::UDP:
	default:
		mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (mySocket == INVALID_SOCKET)
		{
			std::cout << "Failed to create socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			return;
		}

		switch (aHandleType)
		{
		case HandleType::Client:
		{
			//Set udpSocket to non blocking
			// This way recvfrom will not wait until it gets a message
			u_long iMode = true;
			ioctlsocket(mySocket, FIONBIO, &iMode);
			break;
		}
		case HandleType::Server:
		default:
			break;

		}
		break;
	}


	switch (aHandleType)
	{
	case HandleType::Client:
		std::cout << "Message handler used as client!\n";
		myConnection.sin_family = AF_INET;
		myHostingFlag = false;
		myConnectedFlag = true;
		break;
	case HandleType::Server:
		myHostingFlag = true;
		myConnectedFlag = false;
		break;
	default:
		break;
	}

	std::cout << "Initialized!\n";
}

MessageHandler::~MessageHandler()
{
	WSACleanup();
}

const bool MessageHandler::TryConnect(std::wstring anIP, const uint32_t& aPort, const UserInfo& someInfo, const uint8_t aRetryCount)
{
	myHostingFlag = false;
	myConnectedFlag = false;
	if (aRetryCount == 0)
	{
		std::cout << "Ran out of attempts. Assuming that server is offline!\n";
		return false;
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::string serverIP = std::string(anIP.begin(), anIP.end());
	std::cout << "Connecting to (IP:" << serverIP << ", PORT:" << std::to_string(aPort) << ") ...";



	InetPton(AF_INET, anIP.c_str(), &myConnection.sin_addr.s_addr);
	myConnection.sin_port = htons(aPort);

	Client targetServer;
	targetServer.myName = (std::string("Server_") + serverIP).c_str();
	targetServer.myID = uint8_t(-1);
	targetServer.myAddress = myConnection;

	if (!Send(DataType::UserInfo, someInfo, targetServer))
	{

		std::cout << "Failed sending to server!\n";
		return false;
	}

	DataType type;
	UserInfo serverInfo;
	if (Recieve(type, serverInfo, targetServer))
	{
		myConnectedFlag = true;
		std::cout << "Connected!\n";
		return false;
	}


	std::cout << " Failed! Trying again\n";
	return TryConnect(anIP, aPort, someInfo, aRetryCount - 1);


}

const bool MessageHandler::Host(const uint32_t& aPort)
{
	myConnectedFlag = false;

	std::cout << "Message handler used as server!\n";
	std::cout << "Hosting to (PORT: " << std::to_string(aPort) << ")\n";

	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = INADDR_ANY;

	myAddress.sin_port = htons(aPort);

	if (bind(mySocket, reinterpret_cast<sockaddr*>(&myAddress), sizeof(myAddress)) == SOCKET_ERROR)
	{
		
		//HANDLE IT
		return false;
	}
	return myHostingFlag;
}
