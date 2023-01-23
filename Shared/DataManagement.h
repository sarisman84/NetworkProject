#pragma once
#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string_view>
#include <unordered_map>

#include <iostream>

#include "EnumTypes.h"

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




struct UserInfo
{
	EventType myEventType;
	char myName[510];
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
