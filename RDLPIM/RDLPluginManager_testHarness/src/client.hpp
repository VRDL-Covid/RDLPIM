#pragma once

#include<vector>
#include<iostream>
#include"socketConnection.hpp"


class client
{
private:
	static int nextID;
public:
	int ID;
	int port;
	connectionObject connection;
	static std::vector<client*> clients;

	//members


	//methods
	void connect();
	void kill();


	client(int iPort);
	client();
	~client();
};

