#pragma once
#include"connectionOptions.h"
#include"socketConnection.hpp"
#include"portPool.hpp"
#include"buffer.hpp"
#include"ClientManager/client.hpp"

class connectionManager
{

private:
	connectionObject listener;
	portPool ports;

public:	
	void connectToClient();
	void disconnectClient(client* user);
	connectionManager();
	~connectionManager();

};