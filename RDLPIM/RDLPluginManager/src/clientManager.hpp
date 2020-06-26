#pragma once
#include"client.hpp"
#include <stdio.h>
#include <iostream>
#include"buffer.hpp"
#include"requestHandler.hpp"
#include<mutex>


class clientManager
{
public:

	static std::mutex clientDB_lock;
	static std::vector<client*> clients;


	int checkForIncoming(client* client, buffer* output);
	void publishMessage(client* client, const buffer &output);
	static void publishMessage(int ID, const buffer &output);
	static void sendMessage(int ID, const buffer &output);
	static void broadCast(const buffer &output);
	void worker(std::mutex* jobVector);

	clientManager();
	~clientManager();
};

