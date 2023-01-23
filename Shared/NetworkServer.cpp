#include "pch.h"
#include "NetworkServer.h"

NetworkServer::NetworkServer(const uint8_t aPort) : NetworkEntity()
{
	myAdress.sin_family = AF_INET;
	myAdress.sin_addr.s_addr = INADDR_ANY;

	myAdress.sin_port = htons(aPort);

	if (bind(mySocket, reinterpret_cast<sockaddr*>(&myAdress), sizeof myAdress) == SOCKET_ERROR)
	{
		std::cout << "Failed to bind socket." << std::endl;
		std::cout << "Error: " << WSAGetLastError() << std::endl;
		return;
	}
	myIsRunningFlag = true;
	myInputThread = std::thread([this]()
		{
			while (myIsRunningFlag)
			{
				char c;
				std::cin.get(c);
				if (c == 'q')
				{
					myIsRunningFlag = false;
					closesocket(mySocket);
				}
			}
		});


	std::cout << "Initialized Server!" << std::endl;
}

NetworkServer::~NetworkServer()
{
	myInputThread.join();
	WSACleanup();
}

const bool NetworkServer::Update()
{
	Buffer response;
	Client potentialClient;

	DataType type;
	Buffer recievedData;
	int someExtraData;

	if (!Recieve(potentialClient.myAddress, recievedData, &someExtraData))
	{
		return myIsRunningFlag;
	}

	recievedData.ReadData(type);

	switch (type)
	{
	case DataType::UserInfo:
	{
		bool joinFlag;
		int nameSize;
		char name[506];

		recievedData.ReadData(joinFlag);
		recievedData.ReadData(nameSize);
		recievedData.ReadData(name, nameSize);
		if (joinFlag)
		{
			if (!ExistsUserName(name))
			{
				response.WriteData(1);
				response.WriteData(myClients.size());
				potentialClient.myName = name;
				myClients.push_back(potentialClient);
				std::cout << "User joined: " << potentialClient.myName << std::endl;
			}
			else
			{
				std::cout << "User name " << name << " taken! Rejecting join request!" << std::endl;
				response.WriteData(0);
			}
		}

		Send(potentialClient.myAddress, response);

		break;
	}


	case DataType::Message:
		int messageSize;
		char message[510];
		char name[510];

		int user;

		recievedData.ReadData(user);
		recievedData.ReadData(messageSize);
		recievedData.ReadData(message, messageSize);




		for (size_t i = 0; i < myClients.size(); i++)
		{

			if (user == i)
				continue;
			memcpy(name, myClients[user].myName.c_str(), 510);
			response.Clear();
			response.WriteData(DataType::Message);
			response.WriteData(messageSize);
			response.WriteData(message, messageSize);
			response.WriteData((int)strlen(name) + 1);
			response.WriteData(name, (int)strlen(name) + 1);
			Send(myClients[i].myAddress, response);
		}
		break;
	case DataType::Command:
		std::cout << "Some Command!" << std::endl;
		break;
	case DataType::Debug:
		std::cout << "Some Debug!" << std::endl;
		break;
	default:
		break;
	}

	return myIsRunningFlag;
}

const bool NetworkServer::ExistsUserName(const std::string& aName)
{
	auto it = std::find_if(myClients.begin(), myClients.end(), [aName](const Client& aClient) { return aClient.myName == aName; });
	return it != myClients.end();
}

const bool NetworkServer::ExistsUserPort(const uint8_t aPort)
{
	auto it = std::find_if(myClients.begin(), myClients.end(), [aPort](const Client& aClient) { return aClient.myPort == aPort; });
	return it != myClients.end();
}

const size_t NetworkServer::NameToIndex(const std::string& aName)
{
	auto it = std::find_if(myClients.begin(), myClients.end(), [aName](const Client& aClient) { return aClient.myName == aName; });
	return std::distance(myClients.begin(), it);
}

const size_t NetworkServer::PortToIndex(const uint8_t aPort)
{
	return size_t();
}
