#include "rdlpch.h"
#include "clientManager.hpp"
#include"ConnectionManager/connectionManager.hpp"


std::mutex clientManager::clientDB_lock;
std::vector<Ref<Client>> clientManager::clients;

clientManager* clientManager::s_Instance = nullptr;

void clientManager::worker(std::mutex* jobVectorMutex)
{
	
	Buffer inbuff;
	int bytes = 0;
	Buffer test;
	

	while (true) {

		clientManager::clientDB_lock.lock();
		std::vector<Ref<Client>>::iterator it = clientManager::clients.begin();
		
		for (it; it != clients.end(); it++) {
			if (!((bytes = checkForIncoming(*it, &inbuff)) >= 0)) {

				RemoveClient_impl(*it);
				break;
			}
			else {
				if (bytes > 0) {
					Buffer IDdata;

					char temp[4];
					memcpy(temp, &((*it)->ID), sizeof(int));

					IDdata.set(temp, sizeof(int));
					inbuff.prepend(IDdata);

					std::lock_guard<std::mutex>(*jobVectorMutex);
					requestHandler::addToQue(inbuff);
				}
			}
		}
		clientManager::clientDB_lock.unlock();
	}
}


int clientManager::checkForIncoming(Ref<Client> client, Buffer* output)
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

void clientManager::publishMessage(Ref<Client> iclient, const Buffer &output)
{
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (iclient->ID != (*it)->ID) {
			if (!(*it)->connection.Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::publishMessage(int ID, const Buffer &output)
{
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID != (*it)->ID) {
			if (!(*it)->connection.Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::sendMessage(int ID, const Buffer &output)
{
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID == (*it)->ID) {
			if (!(*it)->Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::broadCast(const Buffer &output)
{
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (!(*it)->connection.Send(output))
			RemoveClient(*it);
	}
}

bool clientManager::AddClient_impl(const Ref<Client>& newClient)
{
	clientDB_lock.lock();
	clients.push_back(newClient);
	clientDB_lock.unlock();

	return PROPAGATE_EVENT;
}

bool clientManager::RemoveClient_impl(Ref<Client>& client)
{
	OnClientDisconnect.raiseEvent(client);
	RequestHeader reqHead;
	Buffer outbuffer;
	Buffer data;
	Buffer ID(std::to_string(client->ID).c_str());

	data.set("Client:");
	data.append(ID);
	data.append(" has disconnected");

	reqHead.SetCommand(Commands::Info);
	reqHead.SetSize(data.size);

	outbuffer = reqHead.Serialise();
	outbuffer.append(data);

	std::cout << "client:" << client->ID << " has disconnected" << std::endl;

	publishMessage(client, outbuffer);

	std::vector<Ref<Client>>::iterator it = clients.begin();
	for (; it != clients.end();it++) {
		if ((*it)->ID == client->ID) {
			clients.erase(it);
			break;
		}
	}

	return PROPAGATE_EVENT;
}

clientManager::clientManager()
{
	
}


clientManager::~clientManager()
{

}

void clientManager::Init()
{
	connectionManager::GetInstance()->onNewConnection.subscribe(BIND_EVENT_FN1(clientManager::AddClient_impl));
}
