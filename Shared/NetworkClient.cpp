#include "pch.h"
#include "NetworkClient.h"
#include <cstring>

NetworkClient::NetworkClient(const std::wstring& anIP, const uint8_t aPort) : NetworkEntity()
{
	u_long iMode = true;
	ioctlsocket(mySocket, FIONBIO, &iMode);

	myServerAddress.sin_family = AF_INET;

	InetPton(AF_INET, anIP.c_str(), &myServerAddress.sin_addr.s_addr);
	myServerAddress.sin_port = htons(aPort);

	myHasJoinedFlag = false;
	myIsRunning = true;
	myHasMessageFlag = false;


	bool assignName = false;
	bool validating = false;
	char name[506];
	std::cout << "Enter user name:";

	while (!assignName)
	{
		if (validating)
		{
			Buffer response;
			int extraData;
			while (!Recieve(myServerAddress, response, &extraData));
			int flag;
			response.ReadData(flag);


			if (flag == 0)
			{
				validating = false;
				std::cout << "Name taken! Try again!\nEnter user name:";
			}
			else if (flag == 1)
			{
				response.ReadData(myID);

				myHasJoinedFlag = true;
				validating = false;
				assignName = true;
			}

			continue;
		}


		std::cin.getline(name, 506);
		int size = (int)strlen(name);
		if (size > 0)
		{


			name[size] = '\0';

			Buffer buffer;

			buffer.WriteData(DataType::UserInfo);
			buffer.WriteData(true);
			buffer.WriteData(size + 1);
			buffer.WriteData(name, size + 1);

			Send(myServerAddress, buffer);
			memset(name, 0, sizeof(name));
			validating = true;
		}
		else
			std::cout << "Enter a valid name:";
	}







	myInputThread = std::thread([this]()
		{
			while (myIsRunning)
			{
				while (myHasMessageFlag)
					std::this_thread::yield();
				std::cin.getline(myMessage, 512);
				myMessage[strlen(myMessage)] = '\0';
				myHasMessageFlag = true;
			}
		});

	std::cout << "Initialized Client!" << std::endl;

}

NetworkClient::~NetworkClient()
{
	myInputThread.join();
	WSACleanup();
}

const bool NetworkClient::Update()
{
	if (myHasJoinedFlag)
	{
		if (myHasMessageFlag)
		{
			if (strcmp(myMessage, "quit") == 0)
			{
				myIsRunning = false;
				return myIsRunning;
			}

			Buffer response;
			response.WriteData(DataType::Message);
			response.WriteData(myID);
			response.WriteData((int)strlen(myMessage) + 1);
			response.WriteData(myMessage, (int)strlen(myMessage) + 1);
			if (!Send(myServerAddress, response))
			{
				myIsRunning = false;
				return myIsRunning;
			}

			myHasMessageFlag = false;
		}


		DataType type;
		Buffer incomingData;
		int extraData;
		if (!Recieve(myServerAddress, incomingData, &extraData))
		{
			return myIsRunning;
		}

		incomingData.ReadData(type);

		int messageSize;
		int nameSize;
		char message[510];
		char name[510];

		incomingData.ReadData(messageSize);
		incomingData.ReadData(message, messageSize);
		incomingData.ReadData(nameSize);
		incomingData.ReadData(name, nameSize);
		std::string msg = std::string(name) + ": " + std::string(message);

		switch (type)
		{
		case DataType::Message:
			std::cout << msg << std::endl;
			break;
		case DataType::Command:
			std::cout << "Recieved Command!" << std::endl;
			break;
		case DataType::Debug:
			std::cout << "Recieved Debug!" << std::endl;
			break;
		default:
			break;
		}

		return myIsRunning;
	}
}


