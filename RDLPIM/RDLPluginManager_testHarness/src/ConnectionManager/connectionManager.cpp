#include "rdlpch.h"
#include "connectionManager.hpp"



void connectionManager::connectToClient() 
{
	listener.acceptNewConnection();
	
	while (!listener.connected) {
		Sleep(20);
	}
	
	int nextPort = portPool::getPort();

	char buff[MAXBUFFER];
	memset(buff, '\0', MAXBUFFER);
	strcpy_s(buff, "CONNECT=");


	int start = MAXBUFFER - 1;
	while (buff[start] != '=') {
		start--;
	}
	start++;
	
	memcpy(&buff[start], &nextPort, sizeof(nextPort));

	buffer connectionReq(buff,start+4);
	Sleep(0);
	
	//TODO:GWC- send connection message to client and spool up and manage new client object.
}

void connectionManager::disconnectClient(client* user)
{
	int port = user->port;
	delete user;
	portPool::releasePort(port);
}



connectionManager::connectionManager()
{
	listener.setPort(BASE_PORT);
	listener.init();
}


connectionManager::~connectionManager()
{
	WSACleanup();
}
