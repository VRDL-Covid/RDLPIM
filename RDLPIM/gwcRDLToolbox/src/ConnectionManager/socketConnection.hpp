#pragma once
#include"Core/buffer.hpp"
#include<WS2tcpip.h>
#include"connectionOptions.h"

#pragma comment (lib, "Ws2_32.lib")

#pragma comment (lib, "Ws2_32.lib")




// Author: Guy Collins
// Date: 20/01/2020
// Description: An object to handle asynchronus SOCKET comms.
class connectionObject
{
private:
	WSADATA wsData;
	sockaddr_in serverHints, clientHints;
	int sizeServerHints, sizeClientHints;
	int port;
	char ip[16];
	SOCKET servSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	FD_SET readSet;
	FD_SET writeSet;
public:
	bool connected;

	//members




	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: Method to initialise WSA
	void init();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: method to accept an incomming connection from a client
	void acceptNewConnection();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: method to connect to a remote server.
	int connectToServer();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: method to close handle socket.
	void closeSocket();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: method of shutdown WSA services cleanly.
	void cleanUp();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: method to recieve incoming data into a char array or buffer object.
	int recieve(char* out, int size);
	int recieve(Buffer* buff);

	int Send(char* out, int size);
	int Send(const Buffer &inp);
	int Send(const char* out);

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: Setter methods for IP and Port if not initialised at the constructor.
	void setIP(const char* inp);
	void setPort(int _port);

	//TODO:GWC - get status of socket, is it still valid to talk over? [dpnt! getsockopt() method]
	//bool clientStatus();

	void setNonBlocking(bool state);

	//TODO:GWC - check there is something to be read to prevent blocking...
	bool canRead();
	bool canSend();

	// Author: Guy Collins
	// Date: 21/01/2020
	// Description: constructor and destructor overloads.
	connectionObject();
	connectionObject(SOCKET skt);
	connectionObject(int _port, const char* _ip);
	connectionObject(int _port);
	~connectionObject();
};

