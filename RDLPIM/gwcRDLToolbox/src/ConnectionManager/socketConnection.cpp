#include "rdlpch.h"
#include "socketConnection.hpp"


void connectionObject::init()
{
	PROFILE_FUNCTION();
	//initialise connection state
	connected = false;

	////////////////////
	//initialise winsock
	if (servSocket != INVALID_SOCKET) {
		closesocket(servSocket);
		WSACleanup();
		servSocket = INVALID_SOCKET;
	}


	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);

	if (wsOK != 0) {
		std::cerr << "cant initialise winsock WSA Error:" << WSAGetLastError() << std::endl;
		return;
	}

	//////////////////////////////////
	// Create server connector socket
	servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (servSocket == INVALID_SOCKET) {
		std::cerr << "Unable to start a socket:" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}


	/////////////////////////////////////////
	//bind the ip and port to socket address
	serverHints.sin_family = AF_INET;
	serverHints.sin_port = htons(port);
	serverHints.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(servSocket, (sockaddr*)&serverHints, sizeof(serverHints));


	//////////////////////////////////////////////////////////////////////////////////
	//Initialise read and write sets to test socket states, required for asych sockets
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);

}


void connectionObject::acceptNewConnection()
{
	PROFILE_FUNCTION();
	if (clientSocket != INVALID_SOCKET) {
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
	}

	/////////////////////////////////
	//Tell winsock its for listening
	listen(servSocket, SOMAXCONN);

	/////////////////////
	//accept single connection

	sizeClientHints = sizeof(clientHints);

	clientSocket = accept(servSocket, (sockaddr*)&clientHints, &sizeClientHints);


	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "unable to connect to client, network permission issue? WSA Error:" << WSAGetLastError() << std::endl;
		return;
	}
	else {
		FD_SET(clientSocket, &readSet);
		FD_SET(clientSocket, &writeSet);
		connected = true;
	}


	//TODO:GWC - make DNS look up functional....
	char host[NI_MAXHOST]; //dns lookup of clients PC name.... might be useful...
	char service[NI_MAXHOST]; //the port that the client is connected over..... again... might be useful...

	memset(host, '\0', sizeof(host));
	memset(service, '\0', sizeof(service));

	//do dns lookup
	if (getnameinfo((sockaddr*)&clientHints, sizeof(clientHints), host, sizeof(host), service, sizeof(service), 0) == 0) {
		std::cout << host << " has connected on port: " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &clientHints.sin_addr, host, sizeof(host));
		std::cout << host << " has connected on port: " <<
			ntohs(clientHints.sin_port) << std::endl;
	}
}

int connectionObject::connectToServer()
{
	if (clientSocket != INVALID_SOCKET) {
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Failed to creat client socket: WSA Error " << WSAGetLastError() << std::endl;
	}

	if ((port == -1) || (ip[0] == '\0')) {
		std::cerr << "connection parameters of host are not set port: " << port << " IP: " << ip << std::endl;
		return -1;
	}

	clientHints.sin_family = AF_INET;
	clientHints.sin_port = htons(port);
	inet_pton(AF_INET, ip, &clientHints.sin_addr);

	int WSAStat = connect(clientSocket, (sockaddr*)&clientHints, sizeof(clientHints));

	if (WSAStat == SOCKET_ERROR) {
		std::cerr << "Unable to connect to server, WAS Error:" << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		return -2;
	}
	else {

		FD_SET(clientSocket, &readSet);
		FD_SET(clientSocket, &writeSet);
		connected = true;
	}

	return 0;
}

void connectionObject::closeSocket()
{
	PROFILE_FUNCTION();
	closesocket(servSocket);
}

void connectionObject::cleanUp()
{
	//////////////////
	//shutdown winsock
	WSACleanup();
}

int connectionObject::recieve(char* out, int size)
{
	std::lock_guard<std::mutex> lock(m_IOLock);
	//////////////////
//while loop recv
	memset(out, '\0', size);

	//wait for client to send data.
	int bytesRec = recv(clientSocket, out, size, 0);
	if (bytesRec == SOCKET_ERROR) {
		std::cerr << "Error in reciving from client: WSA Error:" << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		return 0;
	}

	//TODO:GWC add in error checking
	//send(clientSocket, out, bytesRec + 1, 0);

	return bytesRec;
}

int connectionObject::recieve(Buffer& buff)
{
	std::lock_guard<std::mutex> lock(m_IOLock);
	//////////////////
	//while loop recv
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "unable to send, invalid socket being used.  WSA Error: " << WSAGetLastError() << std::endl;
		return 0;
	}

	char temp[1024];
	int bytesRecieved = 0;
	int partialBytes;
	u_long incoming;

	memset(temp, '\0', sizeof(temp));

	//wait for client to send data.
	ioctlsocket(clientSocket, FIONREAD, &incoming);
	partialBytes = recv(clientSocket, temp, incoming, 0);

	if (partialBytes == SOCKET_ERROR) {
		std::cerr << "Error in reciving from client... quiting" << std::endl;
		return -1;
	}

	int bytesExpected = *(int*)&temp[0];
	bytesRecieved += partialBytes;

	buff.set(temp, partialBytes);
	buff.stripHead(4);

	while (bytesRecieved < bytesExpected) {
		int iResult = ioctlsocket(clientSocket, FIONREAD, &incoming);

		if (iResult) {
			switch (iResult) {
				case WSANOTINITIALISED: std::cout << "WSANOTINITIALISED" << std::endl; break;
				case WSAENETDOWN: std::cout << "WSAENETDOWN" << std::endl; break;
				case WSAEINPROGRESS: std::cout << "WSAEINPROGRESS" << std::endl; break;
				case WSAENOTSOCK: std::cout << "WSAENOTSOCK" << std::endl; break;
				case WSAEFAULT: std::cout << "WSAEFAULT" << std::endl; break;
				case WSAEINVAL: std::cout << "WSAEINVAL" << std::endl; break;
				default:  std::cout << iResult << std::endl; break;
			}
		}
		else {
			if (incoming > 0) {
				partialBytes = recv(clientSocket, temp, incoming, 0);
				buff.append(Buffer(temp, partialBytes));
				bytesRecieved += partialBytes;
			}
		}
	}

	return bytesRecieved;
}

int connectionObject::Send(char* out, int size)
{
	std::lock_guard<std::mutex> lock(m_IOLock);

	int bytes;

	if (clientSocket != INVALID_SOCKET) {
		bytes = send(clientSocket, out, size, 0);
	}
	else {
		printf("Connection not valid to send message\n");
		return -1;
	}

	return 0;
}


int connectionObject::Send(const char* out)
{
	std::lock_guard<std::mutex> lock(m_IOLock);
	int size = strlen(out);
	int bytes;

	if (clientSocket != INVALID_SOCKET) {
		bytes = send(clientSocket, out, size, 0);
	}
	else {
		printf("Connection not valid to send message\n");
		return -1;
	}

	return 0;
}

int connectionObject::Send(const Buffer &inp)
{
	std::lock_guard<std::mutex> lock(m_IOLock);

	Buffer toSend = inp;

	Buffer header;
	int bytesRemaining = inp.GetSize()+sizeof(int);
	header.set(&bytesRemaining, sizeof(int));
	toSend.prepend(header);
	int bytesSent = 0;
	int partialSize;

	while (bytesRemaining > 0) {

		if (bytesRemaining >= MAXBUFFER) {
			partialSize = MAXBUFFER;
		}
		else {
			partialSize = bytesRemaining;
		}

		if (clientSocket != INVALID_SOCKET) {
			partialSize = send(clientSocket, &toSend.GetContents()[bytesSent], partialSize, 0);
			bytesRemaining -= partialSize;
			bytesSent += partialSize;
		}
		else {
			printf("Connection not valid to send message\n");
			return 0;
		}
	}
	return bytesSent;
}


void connectionObject::setIP(const char* inp)
{
	memset(ip, '\0', sizeof(ip));
	strcpy_s(ip, inp);
}

void connectionObject::setPort(int _port)
{
	port = _port;
}


void connectionObject::setNonBlocking(bool state)
{
	PROFILE_FUNCTION();
	//set non blocking
	unsigned long nonBlocking;

	if (state) {
		nonBlocking = 1;
	}
	else {
		nonBlocking = 0;
	}

	if (ioctlsocket(clientSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR) {
		std::cout << "unable to make socket non blocking. WSA Error:" << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		connected = false;
		return;
	}
}

bool connectionObject::canRead()
{
	WSAPOLLFD clientFD = {};

	clientFD.fd = clientSocket;
	clientFD.events = POLLRDNORM;
	clientFD.revents = 0;
	

	int ret = WSAPoll(&clientFD, 1, CONNECT_TIMEOUT);

	if (ret == SOCKET_ERROR) {
		connected = false;
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		return false;
	}

	if (ret > 0) {
		if (clientFD.revents & (POLLERR || POLLHUP)) {
			connected = false;
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			return false;
		}
		if (clientFD.revents & POLLRDNORM) {
			return true;
		}
	}

	return false;
}

bool connectionObject::canSend()
{
	PROFILE_FUNCTION();
	WSAPOLLFD clientFD = {};

	clientFD.fd = clientSocket;
	clientFD.events = POLLWRNORM;
	clientFD.revents = 0;

	int ret = WSAPoll(&clientFD, 1, CONNECT_TIMEOUT);

	if (ret == SOCKET_ERROR) {
		connected = false;
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
		return false;
	}

	if (ret > 0) {
		if (clientFD.revents & (POLLERR || POLLHUP)) {
			connected = false;
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			return false;
		}
		if (clientFD.revents & POLLWRNORM) {
			return true;
		}
	}

	return false;
}

connectionObject::connectionObject()
{
	port = -1;
	memset(ip, '\0', sizeof(ip));

	clientHints = { 0 };
	connected = false;
	memset(ip, '\0', sizeof(ip));
}

connectionObject::connectionObject(SOCKET skt)
{

	servSocket = skt;

	clientHints = { 0 };
	connected = false;
	memset(ip, '\0', sizeof(ip));
}

connectionObject::connectionObject(int _port, const char* _ip)
{
	port = _port;

	memset(ip, '\0', sizeof(ip));
	strcpy_s(ip, _ip);

	clientHints = { 0 };
	connected = false;
	memset(ip, '\0', sizeof(ip));
}

connectionObject::connectionObject(int _port)
{
	memset(ip, '\0', sizeof(ip));

	port = _port;

	clientHints = { 0 };
	connected = false;
	memset(ip, '\0', sizeof(ip));
}

connectionObject::~connectionObject()
{
	if (servSocket == INVALID_SOCKET) {
		closesocket(servSocket);
	}

	if (clientSocket == INVALID_SOCKET) {
		closesocket(clientSocket);
	}
}
