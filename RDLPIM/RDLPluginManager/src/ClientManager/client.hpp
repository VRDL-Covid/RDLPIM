#pragma once

#include<vector>
#include<iostream>
#include"ConnectionManager/socketConnection.hpp"


class client
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

	void Send(const buffer& output);

	client(int iPort);
	client();
	~client();
};

