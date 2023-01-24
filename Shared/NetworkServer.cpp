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
		struct LogMessage : public MessageContext
		{
			char myUserName[510];
			int myUserNameSize;

			char myLogMessage[510];
			int myLogMessageSize;

		};


		bool joinFlag;
		int nameSize;
		char name[506];

		recievedData.ReadData(joinFlag);
		recievedData.ReadData(nameSize);
		recievedData.ReadData(name, nameSize);

		//define a function that every logging event uses. In turn, this will be used to send messages about what each user does in the server.
		auto spreadLogMessage = [](Client& aClient, MessageContext& someContext)
		{
			LogMessage& msg = someContext.Get<LogMessage>();

			Buffer response;

			response.WriteData(DataType::UserInfo);
			response.WriteData(msg.myUserNameSize);
			response.WriteData(msg.myUserName, msg.myUserNameSize);
			response.WriteData(msg.myLogMessageSize);
			response.WriteData(msg.myLogMessage, msg.myLogMessageSize);

			msg.myServerInstance->Send(aClient.myAddress, response);
		};

		LogMessage log;

		{ //Store current user's name and name size as well as initialize the log message properly.
			memcpy(log.myUserName, potentialClient.myName.c_str(), potentialClient.myName.length());
			log.myUserNameSize = potentialClient.myName.length();
			log.myServerInstance = this;
		}


		if (joinFlag)
		{
			if (!ExistsUserName(name))
			{
				response.WriteData(1);
				response.WriteData(myClients.size());
				potentialClient.myName = name;
				myClients.push_back(potentialClient);
				std::cout << "User joined: " << potentialClient.myName << std::endl;

				{//Message this event to the other existing clients
					
					{ //Log the fact that this user logged in.
						const char* msg = "logged in.";
						memcpy(log.myLogMessage, "logged in.", strlen(msg) + 1);
						log.myLogMessageSize = strlen(msg) + 1;
					}
				

					//Message very client but the new user but the defined spread logic and information context.
					MessageClients(log, spreadLogMessage, myClients.size() - 1);
				}




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
		int user;

		{//Fetch relevant information about the user and its message.
			recievedData.ReadData(user);
			recievedData.ReadData(messageSize);
			recievedData.ReadData(message, messageSize);
		}

		{ //Apply Message to every client but the sender

			//First, create a context to store specific information about a chat message
			struct ChatMessage : public MessageContext
			{
				ChatMessage(const int aMessageSize, char aMessage[510], const int aUser, std::vector<Client>& someClients) : myClients(someClients)
				{
					myMessageSize = aMessageSize;
					memcpy(myMessage, aMessage, 510);
					myUser = aUser;
				}

				int myMessageSize;
				char myMessage[510];
				int myUser;
				std::vector<Client>& myClients;
			};

			//define the response function via a function pointer using lambda
			auto spreadMessage = [](Client& aClient, MessageContext& someContext)
			{
				ChatMessage& msg = someContext.Get<ChatMessage>();

				char name[510];

				memcpy(name, msg.myClients[msg.myUser].myName.c_str(), 510);
				Buffer response;
				response.WriteData(DataType::Message);
				response.WriteData(msg.myMessageSize);
				response.WriteData(msg.myMessage, msg.myMessageSize);
				response.WriteData((int)strlen(name) + 1);
				response.WriteData(name, (int)strlen(name) + 1);
				msg.myServerInstance->Send(aClient.myAddress, response);
			};


			ChatMessage chatMessage{ messageSize, message, user, myClients };
			chatMessage.myServerInstance = this;
			MessageClients(chatMessage, spreadMessage, user);
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
void NetworkServer::MessageClients(MessageContext& someContext, void(*aMessageCallback)(Client& aClient, MessageContext& someContext), const uint8_t anIndex = (uint8_t)(-1))
{
	for (size_t i = 0; i < myClients.size(); i++)
	{
		if (anIndex != -1 && anIndex == i) continue;

		aMessageCallback(myClients[i], someContext);
	}
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
