// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "Network-Shared.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 42000

// Allows us to gracefully exit the program loop.
std::atomic<bool> isRunning = true;

int main()
{
    // We'll need a udpSocket to communicate on.
    SOCKET udpSocket;
    // And we'll need the Winsock data object.
    WSADATA winsockData;

    // Address information where we'll send our data.
    // i.e. the container for the server address.
    sockaddr_in addrServer{};

    // Data buffer that we'll receive data into.
    char socketBuffer[NETMESSAGE_SIZE];

    std::cout << "Starting Winsock...";
    if(WSAStartup(MAKEWORD(2, 2), &winsockData) != 0)
    {
        std::cout << " FAIL!" << std::endl;
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "OK!" << std::endl;

    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(udpSocket == INVALID_SOCKET)
    {
	    std::cout << "Failed to create udpSocket." << std::endl;
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    // Set udpSocket to non blocking
    // This way recvfrom will not wait until it gets a message
    u_long iMode = true;
    ioctlsocket(udpSocket, FIONBIO, &iMode);

    // Tell the system where the server is that we want to talk with.
    addrServer.sin_family = AF_INET;                    // Internetwork. Required for TCP/UDP/etc

    // Unlike on the Server we cannot specify INADDR_ANY since we need to know
    // which destination IP address to use.
	InetPton(AF_INET, TEXT(SERVER_ADDRESS), &addrServer.sin_addr.s_addr);

    addrServer.sin_port = htons(SERVER_PORT);   // The port that the server is listening on
												// htons converts the byte order from host order to
												// network order.

    // Why no bind on client? We leave that to the operating system!
    // By calling sendto we cause Windows to implicitly bind the udpSocket
    // by filling in all the information and giving us a random local port.
    // This then allows us to use recvfrom as we please because the udpSocket
    // is already bound for us in an appropriate way.

    char message[512]{ 0 };
    std::atomic<bool> hasMessage = false;
    std::thread consoleInputThread([&]
        {
            // very basic async input setup... we read input on a different thread
            while (isRunning)
            {
                while (hasMessage) // wait until message is processed before reading the next one;
                    std::this_thread::yield(); 
                std::cin.getline(message, 512);
                hasMessage = true;
            }
        });

    // send a first message to bind the udpSocket
    // after this we will be able to receive messages
    if (sendto(udpSocket, "Hello!", sizeof("Hello!"), 0, reinterpret_cast<sockaddr*>(&addrServer), sizeof(addrServer)) == SOCKET_ERROR)
    {
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Please enter a message (write quit to quit): ";

    while(isRunning)
    {
        if (hasMessage)
        {
            if (strcmp(message, "quit") == 0)
            {
                isRunning = false;
                break;
            }

            if (sendto(udpSocket, message, 512, 0, reinterpret_cast<sockaddr*>(&addrServer), sizeof(addrServer)) == SOCKET_ERROR)
            {
                std::cout << "Error: " << WSAGetLastError() << std::endl;
                isRunning = false;
                break;
            }

            hasMessage = false;
        }

        ZeroMemory(socketBuffer, NETMESSAGE_SIZE);
        int addrServerSize = sizeof(addrServer);
        const int recv_len = recvfrom(udpSocket, socketBuffer, NETMESSAGE_SIZE, 0, (sockaddr*)&addrServer, &addrServerSize);

        if (recv_len == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
        {
            std::cout << "Failed receiving data from udpSocket." << std::endl;
            std::cout << "Error: " << WSAGetLastError() << std::endl;
        }

        if (recv_len > 0)
        {
            std::cout << "Packet from server!" << std::endl;
            std::cout << "Data: " << socketBuffer << std::endl;
        }
    }

    if (sendto(udpSocket, "Bye!", sizeof("Bye!"), 0, reinterpret_cast<sockaddr*>(&addrServer), sizeof(addrServer)) == SOCKET_ERROR)
    {
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}