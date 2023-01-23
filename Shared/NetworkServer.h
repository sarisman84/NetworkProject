#pragma once
#include "NetworkEntity.h"
#include <thread>
struct Client
{
	sockaddr_in myAddress;
	std::string myName;
	uint8_t myPort;
};


class NetworkServer : public NetworkEntity
{
public:
	NetworkServer(const uint8_t aPort);
	~NetworkServer() override;
	const bool Update() override;
private:
	const bool ExistsUserName(const std::string& aName);
	const bool ExistsUserPort(const uint8_t aPort);
	const size_t NameToIndex(const std::string& aName);
	const size_t PortToIndex(const uint8_t aPort);

private:
	sockaddr_in myAdress;
	std::vector<Client> myClients;

	std::thread myInputThread;
	std::atomic<bool> myIsRunningFlag;
};