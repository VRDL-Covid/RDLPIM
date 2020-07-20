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

	static void publishMessage(int ID, const Buffer& output);
	static void sendMessage(int ID, const Buffer& output);
	static void broadCast(const Buffer& output);
	static bool AddClient(const Ref<Client>& newClient) { return GetInstance()->AddClient_impl(newClient); }
	static bool RemoveClient(Ref<Client>& client) { return GetInstance()->RemoveClient_impl(client);}

	int checkForIncoming(Ref<Client> client, Buffer* output);
	void publishMessage(Ref<Client> client, const Buffer &output);
	void worker(bool& work, std::mutex* jobVector);

	const Event<const Ref<Client>&>& GetNewClientEvent() const { return OnNewClient; }
	const Event<const Ref<Client>&>& GetClientDisconnectEvent() const { return OnClientDisconnect; }

	~clientManager();

private:
	clientManager();
	static clientManager* s_Instance;
	bool AddClient_impl(const Ref<Client>& newClient);
	bool RemoveClient_impl(Ref<Client>& client);

private://Events
	Event<const Ref<Client>&> OnClientDisconnect;
	Event<const Ref<Client>&> OnNewClient;
};

