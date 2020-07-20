#include "rdlpch.h"
#include "connectionManager.hpp"

connectionManager* connectionManager::s_Instance = nullptr;

void connectionManager::buildConnectionDetails(Buffer* connectionReq)
{
	PROFILE_FUNCTION();
	int nextPort = portPool::getPort();
	processingClient =  CreateRef<Client>(nextPort);

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
	PROFILE_FUNCTION();
	Buffer cDeets;
	//send connection data packet to new client;
	if (listener.canSend()) {
		buildConnectionDetails(&cDeets);
		listener.Send(cDeets);
	}

	processingClient->connection.acceptNewConnection();
	processingClient->connection.setNonBlocking(true);

	onNewConnection.raiseEvent(processingClient);
}


void connectionManager::connectToClient() 
{
	PROFILE_FUNCTION();
	listener.acceptNewConnection();
	
	while (!listener.connected) {
		Sleep(20);
	}
	
	sendConnectionDetails();
	//TODO:GWC- send connection message to client and spool up and manage new client object.
}

bool connectionManager::ClientDisconnectCallback(const Ref<Client>& user)
{
	user->connection.closeSocket();
	int port = user->port;
	portPool::releasePort(port);
	return false;
}


void connectionManager::worker(bool& work)
{
	
	while (work) {
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

void connectionManager::Init()
{
	PROFILE_FUNCTION();
	clientManager::GetInstance()->GetClientDisconnectEvent().subscribe(BIND_EVENT_FN1(connectionManager::ClientDisconnectCallback));
}


connectionManager::~connectionManager()
{
	WSACleanup();
}
