#pragma once
#include "NetworkEntity.h"
#include <thread>
struct Client
{
	sockaddr_in myAddress;
	std::string myName;
	uint8_t myPort;


	inline bool operator==(const Client& anOther)
	{
		return myPort == anOther.myPort;
	}
};

class NetworkServer;

class NetworkServer : public NetworkEntity
{
public:
	NetworkServer(const uint8_t aPort);
	~NetworkServer() override;
	const bool Update() override;
	
	template<typename Func>
	void MessageClients(Func&& aMessageCallback, const uint8_t aUserToIgnore = uint8_t(-1));
	void OnShutdown();
private:
	const bool ExistsUserName(const std::string& aName);
	const bool ExistsUserPort(const uint8_t aPort);
	const size_t NameToIndex(const std::string& aName);
	const size_t PortToIndex(const uint8_t aPort);
	
	void HandleUserInfo(Buffer&  someRecievedData, Client&  anPotentialClient);
	void HandleMessages(Buffer& someRecievedData);



private:
	sockaddr_in myAdress;
	std::unordered_map<uint8_t, Client> myClients;

	std::thread myInputThread;
	std::atomic<bool> myIsRunningFlag;
};

template<typename Func>
inline void NetworkServer::MessageClients(Func&& aMessageCallback, const uint8_t aUserToIgnore)
{
	for (auto& client : myClients)
	{
		if (aUserToIgnore != uint8_t(-1) && aUserToIgnore == client.first) continue;

		aMessageCallback(client.second);
	}
}
