#include "client.hpp"

std::vector<client*> client::clients;

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
	clients.push_back(this);

	connection.setPort(port);
	connection.init();
}

client::client()
{
	port = -1;

	ID = client::nextID;
	client::nextID++;
	clients.push_back(this);

}

void client::kill()
{
	std::vector<client*>::iterator it = std::find(client::clients.begin(), client::clients.end(), this);
	client::clients.erase(it);
}


client::~client()
{
	kill();
}
