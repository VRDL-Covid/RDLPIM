#pragma once
#include"buffer.hpp"
enum commands { INVALID, DEBUG, ERR, rdlPush, rdlPull, rdlSubscribe, push, pull, subscribe, chat, VOIP, DATA };

class job
{
public:
	int ID;
	commands command;
	buffer data;

	commands getCommand(buffer* rawJob);
	int getClientID(buffer* thisJob);
	void getData(buffer* rawJob);


	job();
	job(buffer* rawJob);


	~job();
};

