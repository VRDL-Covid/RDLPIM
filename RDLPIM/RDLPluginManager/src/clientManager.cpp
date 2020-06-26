#include "clientManager.hpp"
#include<string>

std::mutex clientManager::clientDB_lock;
std::vector<client*> clientManager::clients;

void clientManager::worker(std::mutex* jobVector)
{
	
	buffer inbuff;
	int bytes = 0;
	buffer test;
	

	while (true) {

		clientManager::clientDB_lock.lock();
		std::vector<client*>::iterator it = clientManager::clients.begin();
		
		for (it; it != clients.end(); it++) {
			if (!((bytes = checkForIncoming(*it, &inbuff)) >= 0)) {
				std::cout << "client:" << (*it)->ID << " has disconnected" << std::endl;

				buffer ID(std::to_string((*it)->ID).c_str());
				ID.prepend("INFO|Client");
				ID.append(":has disconnected");
				publishMessage(*it, ID);


				clients.erase(it);
				break;
			}
			else {
				if (bytes > 0) {
					buffer IDdata("=");
					inbuff.prepend(IDdata);

					char temp[4];
					memcpy(temp, &((*it)->ID), sizeof(int));

					IDdata.set(temp, sizeof(int));
					inbuff.prepend(IDdata);

					IDdata.set("ID=");
					inbuff.prepend(IDdata);
					jobVector->lock();
					requestHandler::addToQue(inbuff);
					jobVector->unlock();
				}
			}
		}
		clientManager::clientDB_lock.unlock();
		//Sleep(1);
	}
}


int clientManager::checkForIncoming(client* client, buffer* output)
{
	int bytes = 0;
	if (client->checkStatus()) {
		if (client->connection.canRead()) {
			bytes = client->connection.recieve(output);
		}
	}
	else {
		return -1;
	}

	return bytes;
}

void clientManager::publishMessage(client* iclient, const buffer &output)
{
	std::vector<client*>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (iclient->ID != (*it)->ID) {
			(*it)->connection.Send(output);
		}
	}
}

void clientManager::publishMessage(int ID, const buffer &output)
{
	std::vector<client*>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID != (*it)->ID) {
			(*it)->connection.Send(output);
		}
	}
}

void clientManager::sendMessage(int ID, const buffer &output)
{
	std::vector<client*>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID == (*it)->ID) {

			//TODO:GWC- TODAY!!!! make the send a method on the client which references the connection object within, the return from this can be used to set the status of the client and kill it when... dead....
			(*it)->Send(output);
		}
	}
}

void clientManager::broadCast(const buffer &output)
{
	std::vector<client*>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		(*it)->connection.Send(output);
	}
}



clientManager::clientManager()
{
}


clientManager::~clientManager()
{
}
