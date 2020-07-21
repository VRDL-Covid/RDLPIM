#include "rdlpch.h"
#include "requestHandler.hpp"

std::vector<Ref<job>> requestHandler::m_jobs;
int requestHandler::noJobs = 0;
requestHandler* requestHandler::s_Instance = nullptr;



void requestHandler::addToQue(const Buffer &rawJob)
{
	PROFILE_FUNCTION();
	m_jobs.push_back(CreateRef<job>(rawJob));
	noJobs++;
}

void requestHandler::terminateJob()
{
	PROFILE_FUNCTION();
	requestHandler::noJobs--;
	requestHandler::m_jobs.erase(requestHandler::m_jobs.begin());
}

void requestHandler::processNextJob()
{
	PROFILE_FUNCTION();
	if (requestHandler::noJobs > 0) {

		switch (m_jobs[0]->command) {
		case Info:
			break;

		case push:
			handlePush();
			break;

		case pull:
			handlePull();
			break;

		case subscribe:
			handleSubscribe();
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
			handelError();
			break;
		}

		terminateJob();
	}
}

void requestHandler::processSubscriptions()
{
	PROFILE_FUNCTION();
	RequestHeader reqHead;
	reqHead.SetCommand(Commands::DATA);
	Buffer OutBuf;

	for (auto& sub : m_Subscriptions) {
		int ID = sub.first;
		auto subInfo = sub.second;
		if (subInfo != nullptr) {
			if (subInfo->IsMarkerForSending()) {
				reqHead.SetSize(subInfo->GetOutData().size);

				OutBuf = reqHead.Serialise();
				OutBuf.append(subInfo->GetOutData());

				clientManager::sendMessage(ID, OutBuf);
				sub.second->Clear();
			}
		}
	}
}
void requestHandler::worker(bool& work, std::mutex* jobVectorMutex)
{
	

	while (work) {
		//handle next request
		if (clientManager::clientDB_lock.try_lock()) {
			jobVectorMutex->lock();
			
			processNextJob();
		
			processSubscriptions();

			jobVectorMutex->unlock();
			clientManager::clientDB_lock.unlock();
		}
	}
}

void requestHandler::handleDEBUG()
{
	PROFILE_FUNCTION();
	Buffer message("ID-");
	message.append(std::to_string(m_jobs[0]->ID).c_str());
	message.append(":");
	message.append(m_jobs[0]->data);
	message.nullTerminate();

	clientManager::publishMessage(m_jobs[0]->ID, message);
}

void requestHandler::handleChat()
{
	PROFILE_FUNCTION();
	RequestHeader reqHead;
	Buffer OutBuf;
	Buffer data;

	data.set("ID-");
	data.append(std::to_string(m_jobs[0]->ID).c_str());
	data.append(":");
	data.append(m_jobs[0]->data);
	data.nullTerminate();

	reqHead.SetCommand(Commands::chat);
	reqHead.SetSize(data.size);

	OutBuf = reqHead.Serialise();
	OutBuf.append(data);

	clientManager::publishMessage(m_jobs[0]->ID, OutBuf);
}

void requestHandler::handleRDLPull()
{
	PROFILE_FUNCTION();
	int nameSize = 0;
	int gap = 0;
	int numReqs = 0;
	Buffer tempName("h");
	int it = 0;

	//End if there is no job data
	if (m_jobs[0]->data.size <= 0) {
		return;
	}

	//If there is valid job data, find how many pulls are being requested.
	while (it < m_jobs[0]->data.size) {
		if(m_jobs[0]->data.contents[it] == '=') {
			numReqs++;
		}
		it++;
	}
	numReqs++;


	//build vector of pull requests
	std::vector<DataElement*> reqs;

	//null terminate the raw request data.
	m_jobs[0]->data.nullTerminate();

	//deserialise the request variable names.
	for (int i = 0; i < numReqs; i++) {
		
		it = 0;
		//get next variable name
		while (m_jobs[0]->data.contents[it+gap] != '=' && m_jobs[0]->data.contents[it+gap] != '\0') {
			it++;
		}
		nameSize = it;
		tempName.set(&(m_jobs[0]->data.contents[gap]), nameSize);

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
		message.append(tempReq.Serialise());
	}

	clientManager::sendMessage(m_jobs[0]->ID, message);
}

void requestHandler::handlePush()
{
	PROFILE_FUNCTION();
	if (m_jobs[0]->data.size > 0) {
		DataElementArray PushDataArr;
		PushDataArr.Deserialise(m_jobs[0]->data);

		DataBase* DB = DataBase::GetInstance();

		DB->ModData(PushDataArr);
	}
}

void requestHandler::handlePull()
{
	PROFILE_FUNCTION();
	if (m_jobs[0]->data.size <= 0)
		return;

	RequestHeader reqHeader;
	DataElement dataElement;
	Buffer SendBuffer;
	Buffer Data;

	DataBase* DB = DataBase::GetInstance();
	Buffer jobData = m_jobs[0]->data;
	std::vector<std::string> requestVars;

	//build request array
	while (jobData.size > 0) {
		requestVars.push_back(jobData.PassChunk('{', '}').ToString());
	}

	//get data
	for (auto var : requestVars) {
		dataElement = DB->GetData(var); 
		Data.append(dataElement.Serialise());
	}

	reqHeader.SetCommand(Commands::DATA);
	reqHeader.SetSize(Data.size);

	SendBuffer = reqHeader.Serialise();
	SendBuffer.append(Data);

	clientManager::sendMessage(m_jobs[0]->ID, SendBuffer);

}

void requestHandler::handleSubscribe()
{
	PROFILE_FUNCTION();

	if (m_jobs[0]->data.size <= 0)
		return;

	RequestHeader reqHeader;
	DataElement dataElement;
	Buffer SendBuffer;
	Buffer Data;

	DataBase* DB = DataBase::GetInstance();
	Buffer jobData = m_jobs[0]->data;
	std::vector<std::string> requestVars;

	//build request array
	while (jobData.size > 0) {
		requestVars.push_back(jobData.PassChunk('{', '}').ToString());
	}

#pragma region build subscribe data
	for (auto var : requestVars) {
		if (DB->PointExists(var))
			m_Subscriptions[m_jobs[0]->ID]->AddVar(var);
	}
#pragma endregion

#pragma region Send Current Data state to client
	
	//get data
	for (auto var : requestVars) {
		dataElement = DB->GetData(var);
		Data.append(dataElement.Serialise());
	}

	reqHeader.SetCommand(Commands::DATA);
	reqHeader.SetSize(Data.size);

	SendBuffer = reqHeader.Serialise();
	SendBuffer.append(Data);

	clientManager::sendMessage(m_jobs[0]->ID, SendBuffer);
#pragma endregion




	
}

void requestHandler::handelError()
{
	PROFILE_FUNCTION();
	Buffer errMessage;
	Buffer sendBuffer;
	RequestHeader reqHed;

	errMessage.set("invalid request code: ");
	errMessage.append(std::to_string((int)m_jobs[0]->command).c_str());
	errMessage.append("-- [hint: is your request header properly formatted [int][int](command, bytes of data)] -- job not processed");

	reqHed.SetSize(errMessage.size);
	reqHed.SetCommand(Commands::ERR);

	sendBuffer = reqHed.Serialise();
	sendBuffer.append(errMessage);

	clientManager::sendMessage(m_jobs[0]->ID, sendBuffer);
}

requestHandler::requestHandler()
{
	//get onNewDataEvent
	auto& newClientEvent = clientManager::Get()->GetNewClientEvent();
	auto& clientDisconnectEvent = clientManager::Get()->GetClientDisconnectEvent();


	//initialise callback objects.
	onClientConnectCallback = CreateRef<EventCallback<const Ref<Client>&>>();
	onClientConnectCallback->SetCallback(BIND_EVENT_FN1(requestHandler::ClientConnectedHandler));
	
	onClientDisconnectCallback = CreateRef<EventCallback<const Ref<Client>&>>();
	onClientDisconnectCallback->SetCallback(BIND_EVENT_FN1(requestHandler::ClientDisconnectedHandler));

	//subscribe to new data events;
	newClientEvent.subscribe(onClientConnectCallback);
	clientDisconnectEvent.subscribe(onClientDisconnectCallback);
}

requestHandler::~requestHandler()
{
}

bool requestHandler::ClientDisconnectedHandler(const Ref<Client>& client)
{
	m_Subscriptions.erase(client->ID);

	return PROPAGATE_EVENT;
}

bool requestHandler::ClientConnectedHandler(const Ref<Client>& client)
{
	m_Subscriptions[client->ID] = CreateRef<Subscriptions>();

	return PROPAGATE_EVENT;
}
