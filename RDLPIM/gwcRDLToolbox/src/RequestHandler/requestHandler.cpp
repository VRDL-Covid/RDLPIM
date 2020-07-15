#include "rdlpch.h"
#include "requestHandler.hpp"

std::vector<job*> requestHandler::jobs;
int requestHandler::noJobs = 0;
requestHandler* requestHandler::s_Instance = nullptr;


void requestHandler::printJobs()
{
	//TODO:GWC - make method to print pending jobs.
}

void requestHandler::addToQue(const Buffer &rawJob)
{
	Buffer rawJobCpy = rawJob;
	job* newJob = new job(&rawJobCpy);
	jobs.push_back(newJob);
	noJobs++;
	printJobs();
}

void requestHandler::terminateJob()
{
	requestHandler::noJobs--;
	requestHandler::jobs.erase(requestHandler::jobs.begin());
}

void requestHandler::processNextJob()
{
	switch (jobs[0]->command)
	{
	case Info:
		terminateJob();
		break;

	case rdlPush:
		terminateJob();
		break;

	case rdlPull:
		handleRDLPull();
		break;

	case rdlSubscribe:
		terminateJob();
		break;

	case push:
		handlePush();
		break;

	case pull:
		terminateJob();
		break;

	case subscribe:
		terminateJob();
		break;

	case chat:
		handleChat();
		break;

	case DEBUG:
		handleDEBUG();
		break;

	case VOIP:
		terminateJob();
		break;

	default:
		handelError();
		break;
	}
}

void requestHandler::worker(std::mutex* jobVectorMutex)
{
	while (true) {
		//handle next request
		if (clientManager::clientDB_lock.try_lock()) {
			jobVectorMutex->lock();
			if (requestHandler::noJobs > 0) {
				processNextJob();
			}
			jobVectorMutex->unlock();
			clientManager::clientDB_lock.unlock();
		}

		//Todo:gwc - check subscribed data for changes and send to subscribed clients if it has...
		//check subscribed data  for changes.

		//checkSubscribedData();
	}
}

void requestHandler::handleDEBUG()
{
	Buffer message("ID-");
	message.append(std::to_string(jobs[0]->ID).c_str());
	message.append(":");
	message.append(jobs[0]->data);
	message.nullTerminate();

	clientManager::publishMessage(jobs[0]->ID, message);
	terminateJob();
}

void requestHandler::handleChat()
{
	RequestHeader reqHead;
	Buffer OutBuf;
	Buffer data;

	data.set("ID-");
	data.append(std::to_string(jobs[0]->ID).c_str());
	data.append(":");
	data.append(jobs[0]->data);
	data.nullTerminate();

	reqHead.SetCommand(Commands::chat);
	reqHead.SetSize(data.size);

	OutBuf = reqHead.Serialise();
	OutBuf.append(data);

	clientManager::publishMessage(jobs[0]->ID, OutBuf);
	terminateJob();
}

void requestHandler::handleRDLPull()
{
	int nameSize = 0;
	int gap = 0;
	int numReqs = 0;
	Buffer tempName("h");
	int it = 0;

	//End if there is no job data
	if (jobs[0]->data.size <= 0) {
		terminateJob();
		return;
	}

	//If there is valid job data, find how many pulls are being requested.
	while (it < jobs[0]->data.size) {
		if(jobs[0]->data.contents[it] == '=') {
			numReqs++;
		}
		it++;
	}
	numReqs++;


	//build vector of pull requests
	std::vector<DataElement*> reqs;

	//null terminate the raw request data.
	jobs[0]->data.nullTerminate();

	//deserialise the request variable names.
	for (int i = 0; i < numReqs; i++) {
		
		it = 0;
		//get next variable name
		while (jobs[0]->data.contents[it+gap] != '=' && jobs[0]->data.contents[it+gap] != '\0') {
			it++;
		}
		nameSize = it;
		tempName.set(&(jobs[0]->data.contents[gap]), nameSize);

		DataElement* newRequest = new DataElement(&tempName);
		reqs.push_back(newRequest);
		gap += nameSize + 1;
	}

	//set message command flag to datagram
	Buffer message;
	char TcommandRaw[sizeof(int)];
	Commands Tcommand;
	Tcommand = Commands::DATA;
	memcpy(TcommandRaw, &Tcommand, 4);
	message.set(TcommandRaw, sizeof(int));
	message.append("=");

	//fill request objects with data


	for (int i = 0; i < numReqs; i++) {
		rdlData RDLDATA;
		RDLDATA.init(reqs[i]->m_VarName);

		DataElement tempReq;
		tempReq.set(RDLDATA);
		message.append(tempReq.serialise());
	}

	clientManager::sendMessage(jobs[0]->ID, message);
	//delete the job
	terminateJob();
}

void requestHandler::handlePush()
{
	dataArray PushDataArr;
	DataBase* DB =  DataBase::GetInstance() ;

	PushDataArr.deserialise(jobs[0]->data);

	for (auto element : PushDataArr){
		DB->ModData(*element);
	}

	terminateJob();
}


void requestHandler::handelError()
{
	Buffer errMessage;
	Buffer sendBuffer;
	RequestHeader reqHed;

	errMessage.set("invalid request code: ");
	errMessage.append(std::to_string((int)jobs[0]->command).c_str());
	errMessage.append("-- [hint: is your request header properly formatted [int][int](command, bytes of data)] -- job not processed");

	reqHed.SetSize(errMessage.size);
	reqHed.SetCommand(Commands::ERR);

	sendBuffer = reqHed.Serialise();
	sendBuffer.append(errMessage);

	clientManager::sendMessage(jobs[0]->ID, sendBuffer);
	terminateJob();
}

requestHandler::requestHandler()
{
}

requestHandler::~requestHandler()
{
}
