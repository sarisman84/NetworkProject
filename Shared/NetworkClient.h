#pragma once
#include "NetworkEntity.h"
#include <thread>

class NetworkClient : public NetworkEntity
{
public:
	NetworkClient(const std::wstring& anIP, const uint8_t aPort);
	~NetworkClient() override;
	const bool Update() override;
private:
	sockaddr_in myServerAddress;
	bool myHasJoinedFlag;
	std::thread myInputThread;

	std::atomic<bool> myHasMessageFlag;
	std::atomic<bool> myIsRunning;

	char myMessage[512];
	int myID;
};