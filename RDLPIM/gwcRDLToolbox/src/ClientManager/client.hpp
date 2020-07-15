#pragma once
#include"ConnectionManager/socketConnection.hpp"


class Client
{
private:
	static int nextID;
public:
	int ID;
	int port;
	connectionObject connection;

	bool connected;

	//members


	//methods
	bool checkStatus();

	void connect();

	int Send(const Buffer& output);

	Client(int iPort);
	Client();
	~Client();
};

