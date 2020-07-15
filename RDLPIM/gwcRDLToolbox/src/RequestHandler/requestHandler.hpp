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

	static std::vector<job*> jobs;
	static int noJobs;

	static void addToQue(const Buffer& rawJob);

	static void printJobs();
	
	void worker(std::mutex* jobVector);

	~requestHandler();

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
	void handelError();

	static requestHandler* s_Instance;
};

