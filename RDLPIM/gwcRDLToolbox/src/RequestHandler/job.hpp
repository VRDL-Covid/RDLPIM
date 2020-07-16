#pragma once
#include"Core/buffer.hpp"
enum Commands { None, Info, DEBUG, ERR, rdlPush, rdlPull, rdlSubscribe, push, pull, subscribe, chat, VOIP, DATA };

class job
{
public:
	int ID;
	Commands command;
	Buffer data;

	Commands getCommand(const Buffer& rawJob);
	int getClientID(const Buffer& rawJob);
	void getData(const Buffer& rawJob);


	job();
	job(const Buffer& rawJob);


	~job();
};

