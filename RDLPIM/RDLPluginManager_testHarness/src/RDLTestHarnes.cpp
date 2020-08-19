#include "gwcRDLToolBox.h"
#include"testClient.hpp"


std::mutex coutMutex;

void sendChatMSGs(Buffer& outBuff, bool* send, std::mutex& sockSend)
{
	uint32_t bytes = 0;
	RequestHeader reqHeader;
	Buffer userInData;

	std::cout <<"Broadcast message:";

	std::cin.clear();
	std::cin.sync();

	//get user input
	std::string msg;
	std::getline(std::cin, msg);
	userInData.set(msg.c_str());

	std::lock_guard<std::mutex> lock(sockSend);


	//buildHeader
	bytes = userInData.GetSize();
	reqHeader.SetSize(bytes);
	reqHeader.SetCommand(Commands::chat);

	outBuff = reqHeader.Serialise();

	//build raw job buffer
	outBuff.append(userInData);

	//output bit from thread to send
	*send = true;

}

void SendPushInt(Buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	Buffer name;
	Buffer data;
	RequestHeader reqHeader;
	int value;

	
	std::vector<std::shared_ptr<DataElement>> requests;

	//get  test input stack
	std::cout << "set name = '!' to end pushInt stack" << std::endl << std::endl;
	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {

		//get value
		std::cout << "Value:" << std::endl;
		std::cin >> value;

		auto request = std::make_shared<DataElement>(name);
		request->set(value);
		requests.emplace_back(request);

		std::cout << "variable Name:" << std::endl;
		std::cin >> name;
	}


	//build data package
	for (auto element : requests) {
		data.append(element->Serialise());
	}

	//build header packet
	reqHeader.SetCommand(Commands::push);
	reqHeader.SetSize(data.GetSize());

	std::lock_guard<std::mutex> lock(sockSend);

	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}


void SendPushDouble(Buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	Buffer name;
	Buffer data;
	RequestHeader reqHeader;
	double value;


	std::vector<std::shared_ptr<DataElement>> requests;

	//get  test input stack
	std::cout << "set name = '!' to end pushInt stack" << std::endl << std::endl;
	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {

		//get value
		std::cout << "Value:" << std::endl;
		std::cin >> value;

		auto request = std::make_shared<DataElement>(name);
		request->set(value);
		requests.emplace_back(request);

		std::cout << "variable Name:" << std::endl;
		std::cin >> name;
	}


	//build data package
	for (auto element : requests) {
		data.append(element->Serialise());
	}

	//build header packet
	reqHeader.SetCommand(Commands::push);
	reqHeader.SetSize(data.GetSize());

	std::lock_guard<std::mutex> lock(sockSend);

	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}

void SendPull(Buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	Buffer name;
	Buffer data;
	RequestHeader reqHeader;


	std::vector<std::shared_ptr<DataElement>> requests;

	//get  test input stack
	std::cout << "set name = '!' to end pullInt stack" << std::endl << std::endl;
	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {
		name.prepend("{");
		name.append("}");
		data.append(name);
		std::cin >> name;
	}

	//build header packet
	reqHeader.SetCommand(Commands::pull);
	reqHeader.SetSize(data.GetSize());

	std::lock_guard<std::mutex> lock(sockSend);
	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}

void SentUnSubInt(Buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	Buffer name;
	Buffer data;
	RequestHeader reqHeader;


	std::vector<std::shared_ptr<DataElement>> requests;

	//get  test input stack
	std::cout << "set name = '!' to end Unsubscribe to Int stack" << std::endl << std::endl;
	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {
		name.prepend("{");
		name.append("}");
		data.append(name);
		std::cin >> name;
	}

	//build header packet
	reqHeader.SetCommand(Commands::unsubscribe);
	reqHeader.SetSize(data.GetSize());

	std::lock_guard<std::mutex> lock(sockSend);
	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}

void SentSubInt(Buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	Buffer name;
	Buffer data;
	RequestHeader reqHeader;


	std::vector<std::shared_ptr<DataElement>> requests;

	//get  test input stack
	std::cout << "set name = '!' to end Subscribe to Int stack" << std::endl << std::endl;
	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {
		name.prepend("{");
		name.append("}");
		data.append(name);
		std::cin >> name;
	}

	//build header packet
	reqHeader.SetCommand(Commands::subscribe);
	reqHeader.SetSize(data.GetSize());

	std::lock_guard<std::mutex> lock(sockSend);
	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}

void sender(Buffer& outBuff, bool* send, std::mutex& sockSend)
{
	int option;
	while (true) {
		option = 0;

		{
			std::lock_guard<std::mutex> lock(coutMutex);
			std::cout << "Select a command" << std::endl;
			std::cout << "1) Chat/string test" << std::endl;
			std::cout << "2) push Int Stack" << std::endl;
			std::cout << "3) push Double Stack" << std::endl;
			std::cout << "4) pull Stack" << std::endl;
			std::cout << "5) Subscribe Int Stack" << std::endl;
			std::cout << "6) Unsubscribe Int Stack" << std::endl;
		}


		std::cin >> option;

		switch(option)
		{
		case 1: {std::cin.clear();sendChatMSGs(outBuff, send, sockSend); break; }
		case 2: {std::cin.clear();SendPushInt(outBuff, send, sockSend); break; }
		case 3: {std::cin.clear();SendPushDouble(outBuff, send, sockSend); break; }
		case 4: {std::cin.clear();SendPull(outBuff, send, sockSend); break; }
		case 5: {std::cin.clear();SentSubInt(outBuff, send, sockSend); break; }
		case 6: {std::cin.clear();SentUnSubInt(outBuff, send, sockSend); break; }
		}
		

		//todo: Sleep needs to go, why did i need it?
		Sleep(200);
	}
}

void handleDataPacket(const Buffer& inBuff)
{
	std::cout << "Data Recieved:"<< std::endl;
	
	{
		std::lock_guard<std::mutex> lock(coutMutex);
		inBuff.fullPrint();
	}
}

int testHarness()
{
	//connectionObject client(8000, "10.106.94.39");
	std::mutex sockSend;

	//testClient client1("127.0.0.1", 8000);
	testClient client1("86.140.237.160", 8000);
	client1.getConnectionData();
	client1.connection.setPort(client1.port);

	client1.connection.connectToServer();

	Buffer inBuff;
	Buffer outBuff;

	bool toSend = false;
	bool* toSendPtr = &toSend;

	testClient* clPtr = &client1;

	std::thread senderThread(sender, std::ref(outBuff), toSendPtr, std::ref(sockSend));

	while (true) {

		if (client1.connection.canRead()) {
			RequestHeader reqHead;

			//recieve header and data
			client1.connection.recieve(&inBuff);
			
			//process header
			reqHead.ProcessHeader(inBuff);


			switch (reqHead.GetCommand()) {
			case Commands::data:
				handleDataPacket(inBuff);
				break;
			case Commands::error:
				{
					std::lock_guard<std::mutex> lock(coutMutex);
					std::cout << "ERROR message: ";
					inBuff.fullPrint();
					std::cout << std::endl;
				}
				break;
			case Commands::chat:
				{
					std::lock_guard<std::mutex> lock(coutMutex);
					inBuff.fullPrint();
					std::cout << std::endl;
				}
				break;
			case Commands::Info:
				{
					std::lock_guard<std::mutex> lock(coutMutex);
					std::cout << "Server Info: ";
					inBuff.fullPrint();
				}
				break;
			default:
				{
					std::lock_guard<std::mutex> lock(coutMutex);
					inBuff.fullPrint();
					std::cout << std::endl;
				}
				break;
			}

		}
		else {

			if (toSend && client1.connection.canSend()) {
				
				std::lock_guard<std::mutex> lock(sockSend);
				client1.connection.Send(outBuff);
				toSend = false;

			}
		}


	}

	senderThread.join();

	getchar();
	return 0;
}

int main()
{
	//time to allow VS to start the RDLPIM before the test harness
	Sleep(1000);

	return testHarness();
}