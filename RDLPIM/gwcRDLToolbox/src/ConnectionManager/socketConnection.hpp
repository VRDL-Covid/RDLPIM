#pragma once
#include"Core/buffer.hpp"
#include<WS2tcpip.h>
#include"connectionOptions.h"

#pragma comment (lib, "Ws2_32.lib")

/// @brief The connectionObject is a wrapper around the window SOCKET class
/// @warning This is not platform independant although should be in the future
/// @author Guy Collins
/// ###Example 
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// connectionObject connection;
/// 
/// connection.setIP("127.0.0.1);
/// connection.setPort(8000);
/// 
/// connection.connectToServer();
/// connection.setNonBlocking(true);
/// 
/// Buffer buff;
/// 
/// while(true){
///		if(connection.canRead()){
///			connection.recieve(&buff);
///		
///			buff.fullPrint();
///		} else {
///		
///			Sleep(1000);
///		}
/// }
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class connectionObject
{
public:
	bool connected; ///<Connection Status: True = Connected

	
	/// @brief Method to initialise WSA and connectionObject parameters
	/// 
	/// Initialises:	Winsock
	///					asynchronous data structure
	///					windows server SOCKET object 
	///					
	void init();

	/// @brief Method to put the connectionObject into a connection accept state
	/// 
	/// Method to put the connectionObject into a connection accept state, once a connection has been made a DNS lookup is performed to 
	/// assertain the enpoints name/ip address. Connections are expected on the port set by setPort(). If succesful, the connection is bound to the client SOCKET objectelse a relevant warning is pipped to console.
	void acceptNewConnection();

	/// @brief Method to connect socket to an endpoint
	/// 
	/// The Endpoint is the IP address defined by setIP() and port defined by setPort().  If successful, the connection is bound to the client SOCKET object else a relevant warning is pipped to console.
	/// @return status code.  0 no errors,
	/// -1 endpoint is not defined, 
	/// -2 WSA error occured.
	int connectToServer();

	/// @brief A method to cleanly close the server side socket
	void closeSocket();

	/// @brief Wrapper to WSA cleanup
	void cleanUp();

	/// @brief A method to recieve incoming data
	/// @param out storage for the incoming message
	/// @param size the size of out
	/// @return number of bytes recieved
	/// @warning This method is blocking, upgrade the connection to nonblocking with setNonBlocking(true) and check for incoming before making the read to prevent a block with canRead().
	int recieve(char* out, int size);

	/// @brief A method to recieve incoming data
	/// @param buff pointer to a Buffer for storage for the incoming message
	/// @return number of bytes recieved
	/// @warning This method is blocking, upgrade the connection to nonblocking with setNonBlocking(true) and check for incoming before making the read to prevent a block with canRead().
	/// 
	int recieve(Buffer* buff);

	/// @brief A method to Send data to connected endpoint
	/// @param out char arry containing data to be sent
	/// @param size size of the data to be send in bytes
	/// @return number of byts sent, -1 if client side socket is invalid.
	/// @warning use in conjunction with canSend() and setNonBlocking(true) to prevent block, this method is blocking.
	int Send(char* out, int size);

	/// @brief A method to Send data to connected endpoint
	/// @param inp refernece to a Buffer containing data to be sent
	/// @return number of byts sent, -1 if client side socket is invalid.
	/// @warning use in conjunction with canSend() and setNonBlocking(true) to prevent block, this method is blocking.
	int Send(const Buffer& inp);

	/// @brief A method to Send data to connected endpoint
	/// @param out const char* data to be sent.
	/// @return number of byts sent, -1 if client side socket is invalid.
	/// @warning use in conjunction with canSend() and setNonBlocking(true) to prevent block, this method is blocking.
	int Send(const char* out);

	/// @brief Method to define the IP of the end point.
	/// @param inp char[] representation of the endpoint's IP address
	void setIP(const char* inp);

	/// @brief Methof to define the port of the endpoint
	/// @param _port the port of the endpoint
	void setPort(int _port);

	/// @brief Method to upgrade the connection to an asynchronous socket.  Do this if you want nonblocking behaviour
	/// @param state stat of non-blocking feature.
	void setNonBlocking(bool state);

	/// @brief Method to check if there is data to be read on a asynchronous non blocking connection
	/// 
	/// @warning This must only be used for nonBlocking connections.
	/// @return status of if there is anything to read. True = data to be read, False = no incoming data.
	bool canRead();

	/// @brief Method to check if end point is able to recieve.
	/// 
	/// @warning This must only be used for nonBlocking connections.
	/// @return status of can send. True = endpoint ready to recieve, False endpoint is not in a position to read data.
	bool canSend();

	connectionObject();
	connectionObject(SOCKET skt);
	connectionObject(int _port, const char* _ip);
	connectionObject(int _port);
	~connectionObject();

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

	std::mutex m_IOLock;
	
};

