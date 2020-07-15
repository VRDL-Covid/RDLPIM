#pragma once
#include"connectionOptions.h"
#include"socketConnection.hpp"
#include"portPool.hpp"
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"


class connectionManager
{
public:
	void connectToClient();
	bool ClientDisconnectCallback(const Ref<Client>& user);

	void worker();
	void Init();
	~connectionManager();

	static connectionManager* GetInstance()
	{
		if (s_Instance == nullptr)
			s_Instance = new connectionManager;
		return s_Instance;
	}

	Event<const Ref<Client>> onNewConnection;

private:
	connectionManager();
	connectionObject listener;
	portPool ports;
	void buildConnectionDetails(buffer* connectionReq);
	void sendConnectionDetails();

	static connectionManager* s_Instance;

	Ref<Client> processingClient;
};