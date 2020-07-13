#include "rdlpch.h"
#include"ConnectionManager/socketConnection.hpp"
#include"RequestHandler/reqElement.hpp"
#include"ClientManager/client.hpp"
#include"ConnectionManager/connectionManager.hpp"
#include"RequestHandler/dataArray.hpp"

#include"testClient.hpp"
enum commands { INVALID, DEBUG, ERR, rdlPush, rdlPull, rdlSubscribe, push, pull, subscribe, chat, VOIP, DATA };

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
		commands chat = commands::DEBUG;
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

void sendChatMSGs(buffer& outBuff, bool* send)
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
	commands chat = commands::chat;
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
		commands chat = commands::rdlPull;
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
	commands Cmd = rdlSubscribe;
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

void SendPushInt(buffer& outBuffer, bool* send)
{
	//get variable name
	buffer name;
	buffer serialised;
	int value;

	std::vector<std::shared_ptr<responceElement>> requests;

	std::cout << "set name = '!' to end pushInt stack" << std::endl << std::endl;

	std::cout << "variable Name:" << std::endl;
	std::cin >> name;
	while (name != "!") {

		//get value
		std::cout << "Value:" << std::endl;
		std::cin >> value;

		auto request = std::make_shared<responceElement>(name);
		request->set(value);
		requests.emplace_back(request);

		std::cout << "variable Name:" << std::endl;
		std::cin >> name;
	}

	outBuffer.set("CMD=");
	int tCmd = commands::push;
	buffer bcmd((char*)&tCmd, 4);

	outBuffer.append(bcmd);

	for (auto element : requests) {
		outBuffer.append(element->serialise());
	}

	*send = true;
}

void sender(buffer& outBuff, bool* send, std::mutex* stdStream)
{
	while (true) {
		stdStream->lock();
		SendPushInt(outBuff, send);
		stdStream->unlock();
		//todo: Sleep needs to go, why did i need it?
		Sleep(200);
	}
}

void handleDataPacket(const buffer& inBuff)
{
	dataArray responce;
	responce.deserialise(inBuff);
	for (int i = 0; i < responce.nElements; i++) {
		std::cout << std::endl << responce.data[i]->varName << ":" << std::endl << responce.data[i]->type << std::endl << responce.data[i]->bytes << std::endl;
		if (!strncmp(responce.data[i]->type.contents, "double",6)) {
			std::cout << "value:" << *(double*)responce.data[i]->data << std::endl;
		}

		if (responce.data[i]->type.contents[0] == 'I' && responce.data[i]->bytes == 4) {
			std::cout << "value:" << *(int*)responce.data[i]->data << std::endl;
		}

		if (responce.data[i]->type.contents[0] == 'L' && responce.data[i]->bytes == 1) {
			std::cout << "value:" << (*(bool*)responce.data[i]->data ? "true" : "false") << std::endl;
		}
		std::cout << std::endl;
	}
}

int testHarness()
{
	//connectionObject client(8000, "10.106.94.39");
	std::mutex stdStream;

	testClient client1("127.0.0.1",8000);
	client1.getConnectionData();
	client1.connection.setPort(client1.port);

	client1.connection.connectToServer();

	buffer inBuff;
	buffer outBuff;

	bool toSend = false;
	bool* toSendPtr = &toSend;

	testClient* clPtr = &client1;

	std::thread senderThread(sender, std::ref(outBuff), toSendPtr, &stdStream);

	while (true) {

		if (client1.connection.canRead()) {

			client1.connection.recieve(inBuff);
			
			//get response type;
			commands recivedCommand;
			char temp[4];
			for (int i = 0; i < 4; i++) {
				temp[i] = inBuff.contents[i];
			}
			memcpy(&recivedCommand, temp, sizeof(int));
			inBuff.stripHead('=');


			switch (recivedCommand) {
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
			default:
				std::cout << "unknown Function, message: ";
				inBuff.fullPrint();
				std::cout << std::endl;
				break;
			}

		}
		else {

			if (toSend && client1.connection.canSend()) {
				stdStream.lock();
				outBuff.fullPrint();
				client1.connection.Send(outBuff);
				stdStream.unlock();
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
	return testHarness();
}