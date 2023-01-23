#include "pch.h"
#include "NetworkEntity.h"

NetworkEntity::NetworkEntity()
{
	if (!InitWinSock())
	{
		myInitializationFlag = unsigned char(-1);
		return;
	}
	if (!InitSocket(SocketType::UDP))
	{
		myInitializationFlag = unsigned char(-1);
	}

	std::cout << "Initialized!\n";
}

NetworkEntity::~NetworkEntity()
{
	WSACleanup();
}

const bool NetworkEntity::InitSocket(const SocketType aSocketType)
{
	if (aSocketType == SocketType::UDP)
	{
		mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (mySocket == INVALID_SOCKET)
		{
			std::cout << "Failed to create socket." << std::endl;
			std::cout << "Error: " << WSAGetLastError() << std::endl;
			return false;
		}
		return true;
	}
	return true;
}

const bool NetworkEntity::InitWinSock()
{
	std::cout << "Initializing Message Handler ... ";
	if (WSAStartup(MAKEWORD(2, 2), &myWinSockContext) != 0)
	{
		std::cout << " Failed to initialize winsock2!" << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

const bool NetworkEntity::Recieve(sockaddr_in& anAddress, Buffer& someData, int* someExtraData, bool aShowError)
{
	int addrServerSize = sizeof(anAddress);
	const int recv_len = recvfrom(mySocket, &someData, someData.Size(), 0, (sockaddr*)&anAddress, &addrServerSize);
	if (someExtraData)
		*someExtraData = recv_len;

	if (!(recv_len > 0 && recv_len != SOCKET_ERROR))
	{
		if (aShowError)
		{
			std::cout << "Failed to recieve message!\n";
			std::cout << "Error: " << WSAGetLastError() << std::endl;
		}

		return false;
	}

	return true;
}

const bool NetworkEntity::Send(sockaddr_in& anAddress, Buffer& const someData)
{
	sockaddr_in& targetAddr = anAddress;

	if (sendto(mySocket, &someData, someData.Size(), 0, reinterpret_cast<sockaddr*>(&targetAddr), sizeof(targetAddr)) == SOCKET_ERROR)
	{
		std::string readableErrorMSG = std::to_string(WSAGetLastError());
		auto error = WSAGetLastError();
		if (error == WSAEAFNOSUPPORT)
			readableErrorMSG = std::string("Protocol Family on incoming socket not supported! [") + std::to_string(WSAGetLastError()) + "]";


		std::cout << "Error: " << readableErrorMSG << std::endl;
		return false;
	}
	return true;
}
