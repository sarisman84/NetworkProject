#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string_view>
#include <unordered_map>

#include <iostream>

#define NETMESSAGE_SIZE 512

//namespace MessageHandler
//{
//	const int InitUPDSocket(SOCKET& aSocket);
//	const int InitWinSockData(WSADATA& aWinSockData);
//	const int Send(const std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr);
//	const int Recieve(std::string& someData, SOCKET& aSocket, sockaddr_in& anAddr);
//
//
//	std::string ParseData(std::string_view& aView, const std::string& aSeparator);
//	struct Message
//	{
//		std::string myName;
//		std::string myMessage;
//	};
//
//	Message ParseMessage(const std::string& aMessage);
//
//
//
//
//	
//}
enum class HandleType
{
	Client, Server
};
enum class SocketType
{
	UDP, TDP
};

enum class DataType : uint8_t
{
	UserInfo, Message, Command, Debug
};

enum class EventType : uint8_t
{
	Join, Leave
};

struct UserInfo
{
	EventType myEventType;
	uint8_t myID;
	const char* myName;
};

struct Message
{
	uint8_t myID;
	const char* myMessage;
};


struct Command
{
	const char* myCommand;
	const char* myArguments;
};

struct Debug
{
	uint8_t myID;
	const char* myName;
};

class Buffer
{
public:
	Buffer()
	{
		Reset();
	}
	inline void Reset()
	{
		myCurPosInBuffer = myBuffer;
	}

	inline void Clear()
	{
		//reset buffer
		memset(myBuffer, 0, sizeof(myBuffer));
		Reset();
	}

	inline const size_t Size()
	{
		return sizeof(myBuffer);
	}

	char* operator&()
	{
		return myBuffer;
	}
public:

	template<typename T>
	const bool WriteData(const T& someData)
	{
		// Only standard layout types can be assumed to have a consistent layout in memory across compiler version etc..
		static_assert(std::is_standard_layout<T>(), "Only serialization of standard layout types supported");

		if (myCurPosInBuffer + sizeof(T) >= GetEnd())
			return false;

		memcpy(myCurPosInBuffer, &someData, sizeof(T));
		myCurPosInBuffer += sizeof(T);

		return true;
	}

	template<typename T>
	const bool WriteData(const T* someData, const size_t aCount)
	{
		// Only standard layout types can be assumed to have a consistent layout in memory across compiler version etc..
		static_assert(std::is_standard_layout<T>(), "Only serialization of standard layout types supported");

		if (myCurPosInBuffer + sizeof(T) * aCount >= GetEnd())
			return false;

		memcpy(myCurPosInBuffer, someData, sizeof(T) * aCount);
		myCurPosInBuffer += sizeof(T) * aCount;

		return true;
	}

	template<typename T>
	const bool ReadData(T& someData)
	{
		if (myCurPosInBuffer + sizeof(T) >= GetEnd())
			return false;

		memcpy(&someData, myCurPosInBuffer, sizeof(T));
		myCurPosInBuffer += sizeof(T);

		return true;
	}

	template<typename T>
	const bool ReadData(T* someData, const size_t aCount)
	{
		if (myCurPosInBuffer + sizeof(T) * aCount >= GetEnd())
			return false;

		memcpy(someData, myCurPosInBuffer, sizeof(T) * aCount);
		myCurPosInBuffer += sizeof(T) * aCount;

		return true;
	}
private:
	inline char* GetEnd()
	{
		return myBuffer + sizeof(myBuffer);
	}
	char myBuffer[512];
	char* myCurPosInBuffer;

};

class Client
{
	friend class MessageHandler;
public:
	inline const uint8_t ID() noexcept { return myID; }
	inline const char*& Name() noexcept { return myName; }
private:
	sockaddr_in myAddress;
	const char* myName;
	uint8_t myID;
};



class MessageHandler
{
public: //Init
	MessageHandler(const HandleType aHandleType, const SocketType aSocketType);
	~MessageHandler();
public: //General interface
	inline const bool HasInitialized() const noexcept {
		return myInitializationSuccessFlag;
	}
	const bool TryConnect(std::wstring  anIP, const uint32_t& aPort, const UserInfo& someInfo, const uint8_t aRetryCount = 10);
	const bool Host(const uint32_t& aPort);
	inline bool& IsHosting() noexcept { return myHostingFlag; }
	inline bool& IsConnected() noexcept { return myConnectedFlag; }
public: //Send and Recieve
	template<typename T>
	const bool Send(const DataType aType, const T& someData, Client& const aTargetHandler);
	template<typename T>
	const bool Recieve(DataType& aType, T& someData, Client& aTargetHandler);
public: //Hosting Loop

private: //General
	WSADATA myWinSockContext;
	SOCKET mySocket;
	bool myInitializationSuccessFlag;
private: //Host data
	sockaddr_in myAddress; //Hosting address
	bool myHostingFlag;
private: //Client data
	sockaddr_in myConnection; //Server address from client
	bool myConnectedFlag;
};

template<typename T>
inline const bool MessageHandler::Send(const DataType aType, const T& someData, Client& const aTargetHandler)
{
	Buffer buffer;
	buffer.WriteData(aType);
	buffer.WriteData<T>(someData);

	sockaddr_in& targetAddr = aTargetHandler.myAddress;

	if (sendto(mySocket, &buffer, buffer.Size(), 0, reinterpret_cast<sockaddr*>(&targetAddr), sizeof(targetAddr)) == SOCKET_ERROR)
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

template<typename T>
inline const bool MessageHandler::Recieve(DataType& aType, T& someData, Client& aTargetHandler)
{

	Buffer buffer;
	int addrServerSize = sizeof(aTargetHandler.myAddress);
	const int recv_len = recvfrom(mySocket, &buffer, buffer.Size(), 0, (sockaddr*)&aTargetHandler.myAddress, &addrServerSize);
	buffer.ReadData(aType);
	buffer.ReadData(someData);

	return recv_len > 0 && recv_len != SOCKET_ERROR;
}
