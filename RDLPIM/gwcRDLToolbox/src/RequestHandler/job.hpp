#pragma once
#include"Core/buffer.hpp"
enum Commands { INVALID, DEBUG, ERR, rdlPush, rdlPull, rdlSubscribe, push, pull, subscribe, chat, VOIP, DATA };

class job
{
public:
	int ID;
	Commands command;
	buffer data;

	Commands getCommand(buffer* rawJob);
	int getClientID(buffer* thisJob);
	void getData(buffer* rawJob);


	job();
	job(buffer* rawJob);


	~job();
};

