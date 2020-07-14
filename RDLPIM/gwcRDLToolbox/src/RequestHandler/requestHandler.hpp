#pragma once
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"
#include"job.hpp"
#include "dataArray.hpp"
#include"RDL/rdlData.hpp"
#include "RequestHeader.h"

class requestHandler
{
private:
	void processNextJob();

	void terminateJob();

	//job handlers
	void handleDEBUG();
	void handleChat();
	void handleRDLPull();

	void handlePush();
	void handelError();

public:
	static std::vector<job*> jobs;
	static int noJobs;

	static void addToQue(const buffer& rawJob);

	static void printJobs();
	
	void worker(std::mutex* jobVector);


	requestHandler();
	~requestHandler();
};

