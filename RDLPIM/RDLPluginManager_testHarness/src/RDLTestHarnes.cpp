#include "gwcRDLToolBox.h"
#include"testClient.hpp"

void sendDebugMSGs(buffer &outBuff, bool* send)
{
	//while (true) {
		char userIn[1024];
		int bytes = 0;
		char CBytes[4];

		//get user input
		std::cin.getline(userIn, sizeof(userIn));
		buffer userInData(userIn);

		//build bytes data
		bytes = userInData.size;
		memcpy(&CBytes, &userInData.size, 4);
		buffer bytesData(CBytes, 4);

		//build command data
		char cmd[4];
		char* cmdPtr = &cmd[0];
		buffer commandSend;
		Commands chat = Commands::DEBUG;
		memcpy(cmd, &chat, 4);
		commandSend.set(cmdPtr, 4);

		//build raw job buffer
		outBuff.set("CMD=");
		outBuff.append(commandSend);
		outBuff.append("=DATA=");
		outBuff.append(bytesData);
		outBuff.append(userInData);

		//output bit from thread to send
		*send = true;
	//}
}

void sendChatMSGs(buffer& outBuff, bool* send, std::mutex& sockSend)
{
	char userIn[1024];
	uint32_t bytes = 0;
	RequestHeader reqHeader;

	//get user input
	std::cin.getline(userIn, sizeof(userIn));

	std::lock_guard<std::mutex> lock(sockSend);

	buffer userInData(userIn);

	//buildHeader
	bytes = userInData.size;
	reqHeader.SetSize(bytes);
	reqHeader.SetCommand(Commands::chat);

	outBuff = reqHeader.Serialise();

	//build raw job buffer
	outBuff.append(userInData);

	//output bit from thread to send
	*send = true;

}

void sendRDLPull(buffer& outBuff, bool* send)
{
		char userIn[1024];
		int bytes = 0;
		char CBytes[4];

		//get user input
		std::cin.getline(userIn, sizeof(userIn));
		buffer userInData(userIn);

		//build bytes data
		bytes = userInData.size;
		memcpy(&CBytes, &userInData.size, 4);
		buffer bytesData(CBytes, 4);

		//build command data
		char cmd[4];
		char* cmdPtr = &cmd[0];
		buffer commandSend;
		Commands chat = Commands::rdlPull;
		memcpy(cmd, &chat, 4);
		commandSend.set(cmdPtr, 4);

		//build raw job buffer
		outBuff.set("CMD=");
		outBuff.append(commandSend);
		outBuff.append("=DATA=");
		outBuff.append(bytesData);
		outBuff.append(userInData);

		//output bit from thread to send
		*send = true;

}

void sendRDLSubcribe(buffer& outBuff, bool* send)
{
	char userIn[1024];
	int bytes = 0;
	char CBytes[4];

	//get user input
	std::cin.getline(userIn, sizeof(userIn));
	buffer userInData(userIn);

	//build command data
	char cmd[4];
	char* cmdPtr = &cmd[0];
	buffer commandSend;
	Commands Cmd = rdlSubscribe;
	memcpy(cmd, &Cmd, 4);
	commandSend.set(cmdPtr, 4);

	//Build Tollerance packet
	char tolChar[8];
	double tol = 0.0;
	memcpy(tolChar, &tol, sizeof(double));
	buffer tolData;
	tolData.set(tolChar, sizeof(double));

	//Build delay packet
	char delChar[8];
	double del = 0.5;
	memcpy(delChar, &del, sizeof(double));
	buffer delData;
	delData.set(delChar, sizeof(double));

	//build bytes data
	bytes = (userInData.size) + 20 + 2*(tolData.size) + 2*(delData.size) + 13;
	memcpy(&CBytes, &bytes, 4);
	buffer bytesData(CBytes, 4);

	//build raw job buffer
	outBuff.set("CMD=");
	outBuff.append(commandSend);
	outBuff.append("=DATA=");
	outBuff.append(bytesData);
	outBuff.append(userInData);
	outBuff.append("=Tol=");
	outBuff.append(tolData);
	outBuff.append("=Del=");
	outBuff.append(delData);
	outBuff.append("=");
	outBuff.append("GI_GDS_FT205");
	outBuff.append("=Tol=");
	outBuff.append(tolData);
	outBuff.append("=Del=");
	outBuff.append(delData);



	//output bit from thread to send
	outBuff.fullPrint();
	*send = true;

}

void SendPushInt(buffer& outBuffer, bool* send, std::mutex& sockSend)
{
	//get variable name
	buffer name;
	buffer data;
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
		data.append(element->serialise());
	}

	//build header packet
	reqHeader.SetCommand(Commands::push);
	reqHeader.SetSize(data.size);

	std::lock_guard<std::mutex> lock(sockSend);

	outBuffer = reqHeader.Serialise();
	outBuffer.append(data);
	*send = true;
}

void sender(buffer& outBuff, bool* send, std::mutex& sockSend)
{
	while (true) {

		outBuff.fullPrint();
		//SendPushInt(outBuff, send, sockSend);
		sendChatMSGs(outBuff, send, sockSend);
		//todo: Sleep needs to go, why did i need it?
		Sleep(200);
	}
}

void handleDataPacket(const buffer& inBuff)
{

}

int testHarness()
{
	//connectionObject client(8000, "10.106.94.39");
	std::mutex sockSend;

	testClient client1("127.0.0.1",8000);
	client1.getConnectionData();
	client1.connection.setPort(client1.port);

	client1.connection.connectToServer();

	buffer inBuff;
	buffer outBuff;

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
			case DATA:
				handleDataPacket(inBuff);
				break;
			case ERR:
				std::cout << "ERROR message: ";
				inBuff.fullPrint();
				std::cout << std::endl;
				break;
			case chat:
				inBuff.fullPrint();
				std::cout << std::endl;
				break;
			case Info:
				std::cout << "Server Info: ";
				inBuff.fullPrint();
				break;
			default:
				std::cout << "unknown Function, message: ";
				inBuff.fullPrint();
				std::cout << std::endl;
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