#pragma once
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"
#include"job.hpp"
#include "dataArray.hpp"
#include"RDL/rdlData.hpp"
#include "RequestHeader.h"
#include "DataBase/DataBase.h"

class requestHandler
{
public:
	static requestHandler* GetInstance()
	{
		if (s_Instance == nullptr)
			s_Instance = new requestHandler;
		return s_Instance;
	}

	static std::vector<Ref<job>> jobs;
	static int noJobs;

	static void addToQue(const Buffer& rawJob);

	static void printJobs();
	
	void worker(std::mutex* jobVector);

	~requestHandler();

	bool NewDataEntryHandler(const std::string& data) { std::cout << data << " has been added" << std::endl; return false; }
	Ref<EventCallback<const std::string>> onNewDataEntry;

private:

	requestHandler();
	void processNextJob();

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

	std::unordered_map<std::string, std::vector<int>> m_Subscriptions;
};

