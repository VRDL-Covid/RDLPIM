#include "requestHandler.hpp"

std::vector<job*> requestHandler::jobs;
int requestHandler::noJobs = 0;



void requestHandler::printJobs()
{
	//TODO:GWC - make method to print pending jobs.
}

void requestHandler::addToQue(const buffer &rawJob)
{
	buffer rawJobCpy = rawJob;
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
	case INVALID:
		break;
	case rdlPush:
		break;
	case rdlPull:
		handleRDLPull();
		break;
	case rdlSubscribe:
		handleRDLSubscribe();
		break;
	case push:
		break;
	case pull:
		break;
	case subscribe:
		break;
	case chat:
		handleChat();
		break;
	case DEBUG:
		handleDEBUG();
		break;
	case VOIP:
		break;
	default:
		break;
	}
}

void requestHandler::checkSubscribedData()
{
	//make reference to the subscription database;
	dataBase* DB = DB->getInstance();

	//refresh RDL sourced data and mark any subscribers for sending.
	std::vector<dataElement>::iterator DB_it = DB->points.begin();
	for (DB_it; DB_it != DB->points.end(); DB_it++) {
		DB_it->rdlUpdate();
		DB_it->markSubscriptionsForSending(); 
	}

	// build packets and send them for each client....;

	
	std::vector<client*>::iterator CL_it = clientManager::clients.begin();

	for (CL_it; CL_it != clientManager::clients.end(); CL_it++) {
		buffer message;
		commands command = DATA;
		message.set(&command, sizeof(command));

		buffer stacker;

		DB_it = DB->points.begin();
		
		int subScriptionIndex = -1;
		for (DB_it; DB_it != DB->points.end(); DB_it++) {
			if ((subScriptionIndex = DB_it->findSubscription((*CL_it)->ID))>-1  && DB_it->subscriptions[subScriptionIndex].markedForSending) {
				message.append("=");

				DB_it->data.serialise(&stacker);

				message.append(stacker);
				DB_it->subscriptions[subScriptionIndex].markedForSending = false;

				for (int i = 0; i < DB_it->data.bytes; i++) {
					DB_it->subscriptions[subScriptionIndex].lastValue[i] = DB_it->data.data[i];
				}
				DB_it->subscriptions[subScriptionIndex].setTimeLastSend();

			}
		}

		if (message.size > sizeof(command)) {
			clientManager::sendMessage((*CL_it)->ID, message);
		}
		
	}
}

void requestHandler::worker(std::mutex* jobVector)
{
	while (true) {
		//handle next request
		if (clientManager::clientDB_lock.try_lock()) {
			jobVector->lock();
			if (requestHandler::noJobs > 0) {
				processNextJob();
			}
			jobVector->unlock();
			clientManager::clientDB_lock.unlock();
		}

		//Todo:gwc - check subscribed data for changes and send to subscribed clients if it has...
		//check subscribed data  for changes.

		//checkSubscribedData();
	}
}

void requestHandler::handleDEBUG()
{
	//todo:gwc - need to include a command when not in debug.
	buffer message("ID-");
	message.append(std::to_string(jobs[0]->ID).c_str());
	message.append(":");
	message.append(jobs[0]->data);
	message.nullTerminate();

	clientManager::publishMessage(jobs[0]->ID, message);
	terminateJob();
}

void requestHandler::handleChat()
{
	//todo:gwc - need to include a command when not in debug.
	char cmd[4];
	char* cmdPtr = &cmd[0];
	commands chat = commands::chat;
	memcpy(cmd, &chat, 4);

	buffer message;
	message.set(&cmd, 4);
	message.append("=ID-");
	message.append(std::to_string(jobs[0]->ID).c_str());
	message.append(":");
	message.append(jobs[0]->data);
	message.nullTerminate();

	clientManager::publishMessage(jobs[0]->ID, message);
	terminateJob();
}

void requestHandler::handleRDLPull()
{
	int nameSize = 0;
	int gap = 0;
	int numReqs = 0;
	buffer tempName("h");
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
	std::vector<reqElement*> reqs;

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

		reqElement* newRequest = new reqElement(&tempName);
		reqs.push_back(newRequest);
		gap += nameSize + 1;
	}

	//set message command flag to datagram
	buffer message;
	char TcommandRaw[sizeof(int)];
	commands Tcommand;
	Tcommand = commands::DATA;
	memcpy(TcommandRaw, &Tcommand, 4);
	message.set(TcommandRaw, sizeof(int));
	message.append("=");

	//fill request objects with data


	for (int i = 0; i < numReqs; i++) {
		rdlData RDLDATA;
		RDLDATA.init(reqs[i]->varName);

		reqElement tempReq;
		tempReq.set(RDLDATA);
		buffer serialised;
		tempReq.serialise(&serialised);

		message.append(serialised);	
	}

	clientManager::sendMessage(jobs[0]->ID, message);
	//delete the job
	terminateJob();
}

void requestHandler::handleRDLSubscribe()
{
	int t_it = 0;

	int DBElement = -1;

	dataBase* DB = DB->getInstance();

	//End if there is no job data
	if (jobs[0]->data.size <= 0) {
		terminateJob();
		return;
	}

	//copy job data into a buffer that can be destroyed.
	buffer jobData = jobs[0]->data;
	buffer currentName;


	//while there is data to process...
	//TODO:GWC- handle multiple subscribes at once.
	while (jobData.size > 0) {

		//get next name
		///////////////////
		t_it = 0;
		//get name size;
		while (jobData.contents[t_it] != '=') {
			t_it++;
		}

		//store next name
		char* t_name = (char*)malloc(t_it);
		for (int i = 0; i < t_it; i++) {
			t_name[i] = jobData.contents[i];
		}
		currentName.set(t_name, t_it);
		free(t_name);

		//remove name data header
		jobData.stripHead('=');

		//Find variable in database
		if ((DBElement = DB->findElement(currentName)) < 0) {
			//if doesnt exits format a new entry
			dataElement newEntry(currentName);

			//if variable is calculated by the RDL
			if (newEntry.source == RDL_Process) {
				//build subscribe object.
				DB->points.push_back(newEntry);
				DBElement = DB->findElement(currentName);
			}
		}

		if (DBElement < 0) {
			//return error, variable not available for subscription
			buffer message;
			buffer temp;
			commands command = ERR;
			message.set(&command, 4);

			message.append("=");
			command = commands::rdlSubscribe;
			temp.set(&command, 4);

			message.append(temp);
			message.append("=");
			message.append(currentName);

			clientManager::sendMessage(jobs[0]->ID, message);
		}
		
		//get Tol Data
		double tol = 0;
		double del = 0;

		if (jobData.size > 7) {
			if (!(strncmp(jobData.contents, "Tol=", 4)) || !(strncmp(jobData.contents, "tol=", 4)))
			{
				jobData.stripHead('=');
				memcpy(&tol, jobData.contents, sizeof(double));
				jobData.stripHead(9);
			}
		}

		if (jobData.size > 7) {
			if (!(strncmp(jobData.contents, "Del=", 4)) || !(strncmp(jobData.contents, "del=", 4)))
			{
				jobData.stripHead('=');
					memcpy(&del, jobData.contents, sizeof(double));
					jobData.stripHead(9);
			}
		}

		if (jobData.size > 7) {
			if (!(strncmp(jobData.contents, "Tol=", 4)) || !(strncmp(jobData.contents, "tol=", 4)))
			{
				jobData.stripHead('=');
					memcpy(&tol, jobData.contents, sizeof(double));
					jobData.stripHead(9);
			}
		}
		if (DBElement >= 0) {
			subscription newSub;
			newSub.userID = jobs[0]->ID;
			newSub.tolerance = tol;
			newSub.setTimeDelay(del);
			newSub.lastValue = (char*)realloc(newSub.lastValue, DB->points[DBElement].data.bytes);

			DB->points[DBElement].subscriptions.push_back(newSub);
		}
	}

	//remove completed job
	terminateJob();
}

requestHandler::requestHandler()
{
}

requestHandler::~requestHandler()
{
}
