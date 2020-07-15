#include "rdlpch.h"
#include "client.hpp"

//std::vector<client*> client::clients;
//int client::headCount = 0;

int Client::nextID = 1000;
void Client::connect()
{
	connection.acceptNewConnection();
	connection.setNonBlocking(true);
}

Client::Client(int iPort)
{
	port = iPort;

	ID = Client::nextID;
	Client::nextID++;

	connection.setPort(port);
	connection.init();

}



bool Client::checkStatus()
{
	connected = connection.connected;
	return connected;
}


Client::Client()
{
	port = -1;
	ID = Client::nextID;
	Client::nextID++;
}


int Client::Send(const buffer& output)
{
	int bytes = connection.Send(output);
	if (bytes == 0) {
		connected = false;
	}

	return bytes;
}


Client::~Client()
{
}
