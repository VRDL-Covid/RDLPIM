#pragma once
#include"socketConnection.hpp"
class testClient
{
public:
	int port;
	int ID;
	connectionObject connection;

	void getConnectionData();
	void connectToHost();

	testClient();
	testClient(const char* IP, int port);
	~testClient();
};

