#pragma once
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"
#include"job.hpp"
#include"reqElement.hpp"
#include"RDL/rdlData.hpp"
#include"RequestHandler/dataBase.hpp"

class requestHandler
{
private:
	void processNextJob();
	void checkSubscribedData();
	void terminateJob();

	//job handlers
	void handleDEBUG();
	void handleChat();
	void handleRDLPull();
	void handleRDLSubscribe();
	void handlePush();

public:
	static std::vector<job*> jobs;
	static int noJobs;

	static void addToQue(const buffer& rawJob);

	static void printJobs();
	
	void worker(std::mutex* jobVector);


	requestHandler();
	~requestHandler();
};
