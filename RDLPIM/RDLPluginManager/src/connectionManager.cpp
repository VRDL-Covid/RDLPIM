#include "connectionManager.hpp"

void connectionManager::buildConnectionDetails(buffer* connectionReq)
{
	int nextPort = portPool::getPort();
	processingClient = new client(nextPort);

	//Build command flag
	char buff[MAXBUFFER];
	memset(buff, '\0', MAXBUFFER);
	strcpy_s(buff, "CONNECT=");

	//Build Port data
	int start = MAXBUFFER - 1;
	while (buff[start] != '=') {
		start--;
	}
	start++;
	memcpy(&buff[start], &nextPort, sizeof(nextPort));
	buff[start + 4] = '=';

	//build ID data
	start = MAXBUFFER - 1;
	while (buff[start] != '=') {
		start--;
	}
	start++;
	memcpy(&buff[start], &processingClient->ID, sizeof(processingClient->ID));

	connectionReq->set(buff, start + 9);


}

void connectionManager::sendConnectionDetails()
{
	buffer cDeets;
	//send connection data packet to new client;
	if (listener.canSend()) {
		buildConnectionDetails(&cDeets);
		listener.Send(cDeets);
	}

	processingClient->connection.acceptNewConnection();
	processingClient->connection.setNonBlocking(true);

	clientManager::clientDB_lock.lock();
	clientManager::clients.push_back(processingClient);
	clientManager::clientDB_lock.unlock();
}


void connectionManager::connectToClient() 
{
	listener.acceptNewConnection();
	
	while (!listener.connected) {
		Sleep(20);
	}
	
	sendConnectionDetails();
	//TODO:GWC- send connection message to client and spool up and manage new client object.
}

void connectionManager::disconnectClient(client* user)
{
	int port = user->port;
	delete user;
	portPool::releasePort(port);
}





void connectionManager::worker()
{
	while (true) {
		listener.closeSocket();
		listener.init();
		connectToClient();
		//todo:gwc - send welcom message?
		//processingClient->connection.Send("########################################################\n");
		//processingClient->connection.Send("#                    RDL PLUGIN MANAGER                #\n");
		//processingClient->connection.Send("#                    ==================                #\n");
		//processingClient->connection.Send("#    Author:Guy Collins                                #\n");
		//processingClient->connection.Send("#    Date:29/01/2020                                   #\n");
		//processingClient->connection.Send("#    Version:0.1WIP                                    #\n");
		//processingClient->connection.Send("#                                                      #\n");
		//processingClient->connection.Send("########################################################\n");

	}
	


	//accept a new connection.

	//
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
