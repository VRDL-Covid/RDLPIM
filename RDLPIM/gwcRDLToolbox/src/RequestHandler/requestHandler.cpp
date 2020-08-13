#include "rdlpch.h"
#include "requestHandler.hpp"

int requestHandler::noJobs = 0;
requestHandler* requestHandler::s_Instance = nullptr;



void requestHandler::addToQue(const Buffer &rawJob)
{
	std::lock_guard<std::mutex> lock(m_jobQueMutex);
	PROFILE_FUNCTION();
	Get()->m_jobs.push_back(CreateRef<job>(rawJob));
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
	std::lock_guard<std::mutex> lock(m_jobQueMutex);
	PROFILE_FUNCTION();
	if (requestHandler::noJobs > 0) {

		switch (m_jobs[0]->command) {
		case Commands::Info:
			break;

		case Commands::push:
			handlePush();
			break;

		case Commands::pull:
			handlePull();
			break;

		case Commands::subscribe:
			handleSubscribe();
			break;

		case Commands::unsubscribe:
			handleUnsubscribe();
			break;

		case Commands::chat:
			handleChat();
			break;

		case Commands::DEBUG:
			handleDEBUG();
			break;

		case Commands::VOIP:
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
	reqHead.SetCommand(Commands::data);
	Buffer OutBuf;

	for (auto& sub : m_Subscriptions) {
		int ID = sub.first;
		auto subInfo = sub.second;
		if (subInfo != nullptr) {
			if (subInfo->IsMarkerForSending()) {
				reqHead.SetSize(subInfo->GetOutData().GetSize());

				OutBuf = reqHead.Serialise();
				OutBuf.append(subInfo->GetOutData());

				clientManager::sendMessage(ID, OutBuf);
				sub.second->Clear();
			}
		}
	}
}
void requestHandler::worker(bool& work)
{
	

	while (work) {
		//handle next request
			processNextJob();
			processSubscriptions();
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
	reqHead.SetSize(data.GetSize());

	OutBuf = reqHead.Serialise();
	OutBuf.append(data);

	clientManager::publishMessage(m_jobs[0]->ID, OutBuf);
}

void requestHandler::handlePush()
{
	PROFILE_FUNCTION();
	if (m_jobs[0]->data.GetSize() > 0) {
		DataElementArray PushDataArr;
		PushDataArr.Deserialise(m_jobs[0]->data);

		DataBase* DB = DataBase::GetInstance();

		DB->ModData(PushDataArr);
	}
}

void requestHandler::handlePull()
{
	PROFILE_FUNCTION();
	if (m_jobs[0]->data.GetSize() <= 0)
		return;

	RequestHeader reqHeader;
	DataElement dataElement;
	Buffer SendBuffer;
	Buffer Data;

	DataBase* DB = DataBase::GetInstance();
	Buffer jobData = m_jobs[0]->data;
	std::vector<std::string> requestVars;

	//build request array
	while (jobData.GetSize() > 0) {
		requestVars.push_back(jobData.PassChunk('{', '}').ToString());
	}

	//get data
	for (auto var : requestVars) {
		dataElement = DB->GetData(var); 
		Data.append(dataElement.Serialise());
	}

	reqHeader.SetCommand(Commands::data);
	reqHeader.SetSize(Data.GetSize());

	SendBuffer = reqHeader.Serialise();
	SendBuffer.append(Data);

	clientManager::sendMessage(m_jobs[0]->ID, SendBuffer);

}

void requestHandler::handleSubscribe()
{
	PROFILE_FUNCTION();

	if (m_jobs[0]->data.GetSize() <= 0)
		return;

	RequestHeader reqHeader;
	DataElement dataElement;
	Buffer SendBuffer;
	Buffer Data;

	DataBase* DB = DataBase::GetInstance();
	Buffer jobData = m_jobs[0]->data;
	std::vector<std::string> requestVars;

	//build request array
	while (jobData.GetSize() > 0) {
		requestVars.push_back(jobData.PassChunk('{', '}').ToString());
	}

#pragma region build subscribe data
	for (auto var : requestVars) {
		if (!DB->PointExists(var)) {
			DataElement tmp(var);
			tmp.m_Type = Buffer("INIT");
			DataElementArray tmpArr;
			tmpArr.AddElement(tmp);
			DB->ModData(tmpArr);
		}
			
		m_Subscriptions[m_jobs[0]->ID]->AddVar(var);
	}
#pragma endregion

#pragma region Send Current Data state to client
	
	//get data
	for (auto var : requestVars) {
		dataElement = DB->GetData(var);
		Data.append(dataElement.Serialise());
	}

	reqHeader.SetCommand(Commands::data);
	reqHeader.SetSize(Data.GetSize());

	SendBuffer = reqHeader.Serialise();
	SendBuffer.append(Data);

	clientManager::sendMessage(m_jobs[0]->ID, SendBuffer);
#pragma endregion
	
}

void requestHandler::handleUnsubscribe()
{
	PROFILE_FUNCTION();

	if (m_jobs[0]->data.GetSize() <= 0)
		return;

	Buffer jobData = m_jobs[0]->data;
	std::vector<std::string> requestVars;

	//build request array
	while (jobData.GetSize() > 0) {
		requestVars.push_back(jobData.PassChunk('{', '}').ToString());
	}

#pragma region build subscribe data
	for (auto var : requestVars) {
		m_Subscriptions[m_jobs[0]->ID]->RemVar(var);
	}
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

	reqHed.SetSize(errMessage.GetSize());
	reqHed.SetCommand(Commands::error);

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
	onClientConnectCallback->SetCallback(BIND_EVENT_FN(requestHandler::ClientConnectedHandler));
	
	onClientDisconnectCallback = CreateRef<EventCallback<const Ref<Client>&>>();
	onClientDisconnectCallback->SetCallback(BIND_EVENT_FN(requestHandler::ClientDisconnectedHandler));

	//subscribe to new data events;
	newClientEvent.subscribe(onClientConnectCallback);
	clientDisconnectEvent.subscribe(onClientDisconnectCallback);
}

requestHandler::~requestHandler()
{
}

bool requestHandler::ClientDisconnectedHandler(const Ref<Client>& client)
{
	m_Subscriptions.erase(client->GetID());

	return PROPAGATE_EVENT;
}

bool requestHandler::ClientConnectedHandler(const Ref<Client>& client)
{
	m_Subscriptions[client->GetID()] = CreateRef<Subscriptions>();

	return PROPAGATE_EVENT;
}
