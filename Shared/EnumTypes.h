#pragma once
enum class SocketType : char
{
	UDP, TDP
};

enum class DataType : char
{
	UserInfo, Message, Command, Debug
};

enum class EventType : char
{
	Join, Leave
};