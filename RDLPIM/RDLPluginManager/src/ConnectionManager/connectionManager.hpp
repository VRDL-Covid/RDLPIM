#pragma once
#include"connectionOptions.h"
#include"socketConnection.hpp"
#include"portPool.hpp"
#include"buffer.hpp"
#include"ClientManager/clientManager.hpp"


class connectionManager
{

private:
	connectionObject listener;
	portPool ports;
	void buildConnectionDetails(buffer* connectionReq);
	void sendConnectionDetails();

	client* processingClient;
public:	
	void connectToClient();
	void disconnectClient(client* user);

	void worker();
	connectionManager();
	~connectionManager();

};