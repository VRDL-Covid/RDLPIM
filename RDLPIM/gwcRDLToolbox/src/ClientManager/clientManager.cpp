#include "rdlpch.h"
#include "clientManager.hpp"
#include"ConnectionManager/connectionManager.hpp"


std::mutex clientManager::clientDB_lock;
std::vector<Ref<Client>> clientManager::clients;

clientManager* clientManager::s_Instance = nullptr;

void clientManager::worker(bool& work)
{
	
	Buffer inbuff;
	int bytes = 0;
	Buffer test;
	
	std::vector<Ref<Client>> clientsCPY;
	while (work) {

		{
			std::lock_guard<std::mutex> lock(clientDB_lock);
			clientsCPY = clientManager::clients;
		}
		
		std::vector<Ref<Client>>::iterator it = clientsCPY.begin();

		for (it; it != clientsCPY.end(); it++) {
			if (!((bytes = checkForIncoming(*it, inbuff)) >= 0)) {

				RemoveClient_impl(*it);
					break;
			}
			else {
				if (bytes > 0) {
					//TODO - check if header is properly formatted
					Buffer IDdata;

					char temp[4];
					int _id = (*it)->GetID();
					memcpy(temp, &_id, sizeof(int));
					

					IDdata.set(temp, sizeof(int));
					inbuff.prepend(IDdata);

					requestHandler::Get()->addToQue(inbuff);
				}
			}
		}
		
	}
}


int clientManager::checkForIncoming(Ref<Client> client, Buffer& output)
{
	PROFILE_FUNCTION();
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
	std::lock_guard<std::mutex> lock(clientDB_lock);
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (iclient->GetID() != (*it)->GetID()) {
			if (!(*it)->connection.Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::publishMessage(int ID, const Buffer &output)
{
	std::lock_guard<std::mutex> lock(clientDB_lock);
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID != (*it)->GetID()) {
			if (!(*it)->connection.Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::sendMessage(int ID, const Buffer &output)
{
	std::lock_guard<std::mutex> lock(clientDB_lock);
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (ID == (*it)->GetID()) {
			if (!(*it)->Send(output))
				RemoveClient(*it);
		}
	}
}

void clientManager::broadCast(const Buffer &output)
{
	std::lock_guard<std::mutex> lock(clientDB_lock);
	std::vector<Ref<Client>>::iterator it = clients.begin();

	for (it; it != clients.end(); it++) {
		if (!(*it)->connection.Send(output))
			RemoveClient(*it);
	}
}

bool clientManager::AddClient_impl(const Ref<Client>& newClient)
{
	std::lock_guard<std::mutex> lock(clientDB_lock);
	clients.push_back(newClient);


	OnNewClient.raiseEvent(newClient);

	return PROPAGATE_EVENT;
}

bool clientManager::RemoveClient_impl(Ref<Client>& client)
{
	PROFILE_FUNCTION();
	OnClientDisconnect.raiseEvent(client);
	RequestHeader reqHead;
	Buffer outbuffer;
	Buffer data;
	Buffer ID(std::to_string(client->GetID()).c_str());

	data.set("Client:");
	data.append(ID);
	data.append(" has disconnected");

	reqHead.SetCommand(Commands::Info);
	reqHead.SetSize(data.GetSize());

	outbuffer = reqHead.Serialise();
	outbuffer.append(data);

	std::cout << "client:" << client->GetID() << " has disconnected" << std::endl;

	publishMessage(client, outbuffer);

	std::lock_guard<std::mutex> lock(clientDB_lock);
	std::vector<Ref<Client>>::iterator it = clients.begin();
	for (; it != clients.end();it++) {
		if ((*it)->GetID() == client->GetID()) {
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
	PROFILE_FUNCTION();
	connectionManager::Get()->GetNewConnectionEvent().subscribe(BIND_EVENT_FN(clientManager::AddClient_impl));
}
