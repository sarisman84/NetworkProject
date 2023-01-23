// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <string>

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "Network-Shared.h"
#include "NetworkClient.h"

#define SERVER_ADDRESS L"127.0.0.1"
#define SERVER_PORT 42000

int main()
{
	NetworkClient client(SERVER_ADDRESS, SERVER_PORT);
	while (client.Update());

	return 0;
}