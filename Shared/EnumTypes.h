#pragma once
enum class SocketType : char
{
	UDP, TDP
};

enum class DataType : char
{
	UserInfo, Message, Command, Status
};

enum class EventType : char
{
	Join, Leave
};

