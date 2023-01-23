#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string_view>
#include <unordered_map>

#include <iostream>
#include "EnumTypes.h"
#include "DataManagement.h"

class NetworkEntity
{
public:
	NetworkEntity();
	virtual ~NetworkEntity();
public:
	const bool InitSocket(const SocketType aSocketType);
	const bool InitWinSock();
	virtual const bool Update() = 0;
public:
	const bool Send(sockaddr_in& anAddress, Buffer& const someData);

	const bool Recieve(sockaddr_in& anAddress, Buffer& aBuffer, int* someExtraData, bool aShowError = false);
protected:
	SOCKET mySocket;
	WSADATA myWinSockContext;
	unsigned char myInitializationFlag;

};




