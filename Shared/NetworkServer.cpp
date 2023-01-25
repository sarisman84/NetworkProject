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
	OnShutdown();

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
		HandleUserInfo(recievedData, potentialClient);
		break;
	case DataType::Message:
		HandleMessages(recievedData);
		break;
	case DataType::Command:
		std::cout << "Some Command!" << std::endl;
		break;
	case DataType::Status:
		std::cout << "Some Debug!" << std::endl;
		break;
	default:
		break;
	}

	return myIsRunningFlag;
}

//void NetworkServer::MessageClients(F&& func, const uint8_t anIndex = (uint8_t)(-1))
//{
//	for (size_t i = 0; i < myClients.size(); i++)
//	{
//		if (anIndex != -1 && anIndex == i) continue;
//
//		func(myClients[i]);
//
//	}
//}

const bool NetworkServer::ExistsUserName(const std::string& aName)
{
	for (auto& client : myClients)
	{
		if (client.second.myName == aName)
			return true;
	}

	return false;
}

const bool NetworkServer::ExistsUserPort(const uint8_t aPort)
{
	for (auto& client : myClients)
	{
		if (client.second.myPort == aPort)
			return true;
	}

	return false;
}

const size_t NetworkServer::NameToIndex(const std::string& aName)
{

	for (auto& client : myClients)
	{
		if (client.second.myName == aName)
			return client.first;
	}
	//auto it = std::find_if(myClients.begin(), myClients.end(), [aName](const Client& aClient) { return aClient.myName == aName; });
	//return std::distance(myClients.begin(), it);
	return size_t(-1);
}

const size_t NetworkServer::PortToIndex(const uint8_t aPort)
{
	for (auto& client : myClients)
	{
		if (client.second.myPort == aPort)
			return client.first;
	}

	return size_t(-1);
}

void NetworkServer::HandleUserInfo(Buffer& someRecievedData, Client& anPotentialClient)
{
#pragma region Setup
	Buffer response;

	bool joinFlag;
	someRecievedData.ReadData(joinFlag);

	int nameSize;
	char name[510];

	int logMessageSize;
	char logMessage[510];

	//define a function that every logging event uses. In turn, this will be used to send messages about what each user does in the server.
	auto spreadLogMessage = [this, &nameSize, &name, &logMessageSize, &logMessage](Client& aClient)
	{

		Buffer response;

		response.WriteData(DataType::UserInfo);
		response.WriteData(nameSize);
		response.WriteData(name, nameSize);
		response.WriteData(logMessageSize);
		response.WriteData(logMessage, logMessageSize);

		Send(aClient.myAddress, response);
	};


#pragma endregion

	if (joinFlag) //User joined logic
	{


		someRecievedData.ReadData(nameSize);
		someRecievedData.ReadData(name, nameSize);

		if (!ExistsUserName(name))
		{
			auto newIndex = myClients.size();
			response.WriteData(1);
			response.WriteData(newIndex);
			anPotentialClient.myName = name;
			myClients[newIndex] = anPotentialClient;
			std::cout << "User joined: " << anPotentialClient.myName << std::endl;

			{//Message this event to the other existing clients

				{ //Log the fact that this user logged in.
					const char* msg = "joined.";
					memcpy(logMessage, msg, strlen(msg) + 1);
					logMessageSize = strlen(msg) + 1;
				}


				//Message very client but the new user but the defined spread logic and information context.
				MessageClients(spreadLogMessage, myClients.size() - 1);
			}




		}
		else
		{
			std::cout << "User name " << name << " taken! Rejecting join request!" << std::endl;
			response.WriteData(0);
		}

		Send(anPotentialClient.myAddress, response);
		return;
	}


	{ //User left logic
		int index;
		someRecievedData.ReadData(index);

		auto leftUserName = myClients[index].myName;

		{ //Store current user's name and name size as well as initialize the log message properly.
			memcpy(name, leftUserName.c_str(), leftUserName.length() + 1);
			nameSize = leftUserName.length() + 1;
		}

		myClients.erase(index);


		{//Message this event to the other existing clients

			{ //Log the fact that this user logged out.
				const char* msg = "left.";
				memcpy(logMessage, msg, strlen(msg) + 1);
				logMessageSize = strlen(msg) + 1;
			}


			//Message very client but the new user but the defined spread logic and information context.
			MessageClients(spreadLogMessage);
		}

		std::cout << "User left: " << leftUserName << std::endl;
	}




}

void NetworkServer::HandleMessages(Buffer& someRecievedData)
{
	int messageSize;
	char message[510];
	int user;

	{//Fetch relevant information about the user and its message.
		someRecievedData.ReadData(user);
		someRecievedData.ReadData(messageSize);
		someRecievedData.ReadData(message, messageSize);
	}

	{ //Apply Message to every client but the sender

		//First, create a context to store specific information about a chat message

		//define the response function via a function pointer using lambda
		auto spreadMessage = [this, &messageSize, &message, &user](Client& aClient)
		{

			char name[510];

			memcpy(name, myClients[user].myName.c_str(), 510);
			Buffer response;
			response.WriteData(DataType::Message);
			response.WriteData(messageSize);
			response.WriteData(message, messageSize);
			response.WriteData((int)strlen(name) + 1);
			response.WriteData(name, (int)strlen(name) + 1);
			Send(aClient.myAddress, response);
		};


		MessageClients(spreadMessage, user);
	}
}

void NetworkServer::OnShutdown()
{
	std::cout << "Shutting Down!" << std::endl;
	myIsRunningFlag = false;

	auto shutdownMessage = [this](Client& aClient)
	{
		char message[510];

		Buffer response;

		response.WriteData(DataType::Status);
		const char* msg = "Lost connection to server [Server Shutdown]";

		int messageSize = strlen(msg) + 1;

		memcpy(message, msg, messageSize);
		response.WriteData(messageSize);
		response.WriteData(message, messageSize);
		response.WriteData(false);

		Send(aClient.myAddress, response);

		std::cout << "Sending status to " << aClient.myName << std::endl;

	};


	MessageClients(shutdownMessage);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	myClients.clear();

	myInputThread.join();
	WSACleanup();

}
