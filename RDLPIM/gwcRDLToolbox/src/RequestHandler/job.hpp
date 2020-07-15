#pragma once
#include"Core/buffer.hpp"
enum Commands { None, Info, DEBUG, ERR, rdlPush, rdlPull, rdlSubscribe, push, pull, subscribe, chat, VOIP, DATA };

class job
{
public:
	int ID;
	Commands command;
	Buffer data;

	Commands getCommand(Buffer* rawJob);
	int getClientID(Buffer* thisJob);
	void getData(Buffer* rawJob);


	job();
	job(Buffer* rawJob);


	~job();
};

