#pragma once
#include"ConnectionManager/socketConnection.hpp"

/// A Class to represent the behaviour of a Client connected to the RDLPIM
/// 
/// This class may be used to send messages to a connected client whilst maintaining the status of its connection
/// 
/// @author Guy Collins
/// ### Example
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// Client myClient(8000);
/// myClient.connect();
/// 
/// Buffer message("Hello World");
/// 
/// if(myClient.CheckStatus()){
///		myClient.Send(message);
/// }
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
class Client
{

public:

	bool connected;
	connectionObject connection;

	/// @brief A method to get the port of the connected client
	/// @return int representation of the port
	const int GetPort() const { return port; }
	
	/// @brief A method to get the ID of the connected client
	/// @return int representation of the client's ID
	const int GetID() const { return ID; }

	/// @brief Get the connection status of the client, connected = true, 
	/// @return state of the Client's connection
	bool checkStatus();

	/// @brief put the client into a accetp connections state
	void connect();


	/// @brief Sends a datagram to the connected client
	/// @param output Buffer containing the raw message to be sent
	/// @return the number of bytes sent
	int Send(const Buffer& output);

	/// @brief Constructor overload setting the port to connect over.
	/// @param iPort 
	Client(int iPort);
	Client();
	~Client();

private:
	
	int ID;
	int port;
	static int nextID;
};

