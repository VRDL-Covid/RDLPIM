#pragma once
#include<stdio.h>
#include<iostream>
#include<vector>
#include<mutex>
#include<string>
#include"buffer.hpp"
#include"clientManager.hpp"
#include"job.hpp"
#include"reqElement.hpp"
#include"rdlData.hpp"
#include"dataBase.hpp"




class requestHandler
{
private:
	void processNextJob();
	void checkSubscribedData();
	void terminateJob();

	void handleDEBUG();
	void handleRDLPull();
	void handleRDLSubscribe();

public:
	static std::vector<job*> jobs;
	static int noJobs;

	static void addToQue(const buffer& rawJob);

	static void printJobs();
	
	void worker(std::mutex* jobVector);


	requestHandler();
	~requestHandler();
};
