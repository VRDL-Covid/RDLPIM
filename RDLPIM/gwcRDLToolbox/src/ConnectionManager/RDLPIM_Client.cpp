#include "rdlpch.h"
#include "RDLPIM_Client.hpp"



void rdlpimClient::connectToRDLPIM()
{

	connection.connectToServer();

	char tempBuff[1024];
	char connectDetails[1024];
	memset(tempBuff, '\0', 1024);
	memset(connectDetails, '\0', 1024);

	if (!connection.canRead()) {
		Sleep(CONNECT_TIMEOUT);
	}
	if (connection.canRead()) {
		//recieve raw data;
		if (!connection.recieve(tempBuff, 1024)) {
			std::cerr << "Could not connect to remote host" << std::endl;
			return;
		}
		Buffer rec;
		rec.set(tempBuff, 1024);
		rec.stripHead(4);
		

		for (int i = 0; i < rec.GetSize(); i++) {
			tempBuff[i] = rec.GetContents()[i];
		}

		int i = 0;
		int k = 0;

		while (tempBuff[i] != '=') {
			connectDetails[i] = tempBuff[i];
			i++;
		}
		i++;

		if (!strcmp(connectDetails, "ERROR")) {
			memset(connectDetails, '\0', 1024);
			while (tempBuff[i + k] != '\0') {

				connectDetails[i] = tempBuff[i + k];
				k++;
			}

			std::cerr << "Unable to connect to RDL plugin manager, message from host" << connectDetails << std::endl;
			connection.closeSocket();
			return;
		}

		if (!strcmp(connectDetails, "CONNECT")) {
			//deserialise port data
			memset(connectDetails, '\0', 1024);
			for (int j = 0; j < 4; j++) {
				connectDetails[j] = tempBuff[i];
				i++;
			} 
			i++;
			memcpy(&port, connectDetails, sizeof(port));

			//deserialise ID data
			memset(connectDetails, '\0', 1024);
			for (int j = 0; j < 4; j++) {
				connectDetails[j] = tempBuff[i];
				i++;
			}
			i++;
			memcpy(&ID, connectDetails, sizeof(port));

			connection.setPort(port);

			connection.connectToServer();
			connection.setNonBlocking(true);
		}
	}
	else {
		std::cerr << "Unable to get connection data from host, connection timed out." << std::endl;
	}
}


bool rdlpimClient::CanRead()
{
	return connection.canRead();
}

bool rdlpimClient::CanSend()
{
	return connection.canSend();
}

int rdlpimClient::Recieve(Buffer& buff)
{
	return connection.recieve(buff);
}


int rdlpimClient::Send(const Buffer& buff)
{
	return connection.Send(buff);
}


rdlpimClient::rdlpimClient()
{
	connection.setPort(8000);
	connection.setIP("127.0.0.1");

	connection.init();
}

rdlpimClient::rdlpimClient(const char* IP,int port)
{
	connection.setPort(port);
	connection.setIP(IP);

	connection.init();
}


rdlpimClient::~rdlpimClient()
{
	connection.closeSocket();
	connection.cleanUp();
}
