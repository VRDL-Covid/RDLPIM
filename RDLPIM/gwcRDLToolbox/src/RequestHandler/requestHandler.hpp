#pragma once
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"
#include"job.hpp"
#include "dataArray.hpp"
#include"RDL/rdlData.hpp"
#include "RequestHeader.h"
#include "DataBase/DataBase.h"
#include "RequestHandler/Subscriptions.h"

class requestHandler
{
public: //methods
	static requestHandler* Get()
	{
		if (s_Instance == nullptr)
			s_Instance = new requestHandler;
		return s_Instance;
	}
	~requestHandler();

	static int noJobs;

	static void addToQue(const Buffer& rawJob);
	
	void worker(bool& work, std::mutex& jobVector);

public://callbacks
	bool ClientConnectedHandler(const Ref<Client>& client);
	Ref<EventCallback<const Ref<Client>&>> onClientConnectCallback;

	bool ClientDisconnectedHandler(const Ref<Client>& client);
	Ref<EventCallback<const Ref<Client>&>> onClientDisconnectCallback;


private:

	requestHandler();
	void processNextJob();
	void processSubscriptions();

	void terminateJob();

	//job handlers
	void handleDEBUG();
	void handleChat();
	void handleRDLPull();

	void handlePush();
	void handlePull();
	void handleSubscribe();
	void handelError();

	static requestHandler* s_Instance;

	std::unordered_map<int, Ref<Subscriptions>> m_Subscriptions;

	static std::vector<Ref<job>> m_jobs;
};

