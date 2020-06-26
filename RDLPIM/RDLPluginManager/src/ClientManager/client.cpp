#include "client.hpp"

//std::vector<client*> client::clients;
//int client::headCount = 0;

int client::nextID = 1000;
void client::connect()
{
	connection.acceptNewConnection();
	connection.setNonBlocking(true);
}

client::client(int iPort)
{
	port = iPort;

	ID = client::nextID;
	client::nextID++;

	connection.setPort(port);
	connection.init();

}



bool client::checkStatus()
{
	connected = connection.connected;
	return connected;
}


client::client()
{
	port = -1;
	ID = client::nextID;
	client::nextID++;
}


void client::Send(const buffer& output)
{
	if (connection.Send(output) < 0) {
		connected = false;
	}
}


client::~client()
{
}
