#pragma once
#include"Core/buffer.hpp"
enum class Commands { None, data, chat, push, pull, subscribe, unsubscribe, VOIP, Info, error, DEBUG};

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

