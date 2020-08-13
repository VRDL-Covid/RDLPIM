#pragma once
#include"ConnectionManager/socketConnection.hpp"

/// A Class to represent the behaviour of a Client connected to the RDLPIM
/// 
/// This class may be used to send messages to a connected client. 
class Client
{

public:


	//members
	bool connected;
	connectionObject connection;

	//getters
	const int GetPort() const { return port; }
	const int GetID() const { return ID; }

	//methods
	bool checkStatus();

	void connect();

	int Send(const Buffer& output);

	Client(int iPort);
	Client();
	~Client();

private:
	int ID;
	int port;
	static int nextID;
};

