#pragma once
#include"client.hpp"
#include"Core/buffer.hpp"
#include"RequestHandler/requestHandler.hpp"
#include"Core/Event.h"

class clientManager
{
public:
	void Init();

	static clientManager* GetInstance()
	{
		if (s_Instance == nullptr)
			s_Instance = new clientManager;

		return s_Instance;
	}
	static std::mutex clientDB_lock;
	static std::vector<Ref<Client>> clients;

	static void publishMessage(int ID, const buffer& output);
	static void sendMessage(int ID, const buffer& output);
	static void broadCast(const buffer& output);
	static bool AddClient(const Ref<Client>& newClient) { return GetInstance()->AddClient_impl(newClient); }
	static bool RemoveClient(Ref<Client>& client) { return GetInstance()->RemoveClient_impl(client); }

	int checkForIncoming(Ref<Client> client, buffer* output);
	void publishMessage(Ref<Client> client, const buffer &output);
	void worker(std::mutex* jobVector);

	~clientManager();

	Event<const Ref<Client>> OnClientDisconnect;
	Event<const Ref<Client>> OnNewClient;

private:
	clientManager();
	static clientManager* s_Instance;
	bool AddClient_impl(const Ref<Client>& newClient);
	bool RemoveClient_impl(Ref<Client>& client);
};

