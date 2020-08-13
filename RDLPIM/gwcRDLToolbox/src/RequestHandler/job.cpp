#include "rdlpch.h"
#include "job.hpp"

Commands job::getCommand(const Buffer& rawJob)
{
	memcpy(&command, &rawJob.GetContents()[4], sizeof(int));
	return command;
}

int job::getClientID(const Buffer& rawJob)
{
	memcpy(&ID, &rawJob.GetContents()[0], sizeof(int));
	return ID;
}

void job::getData(const Buffer& rawJob)
{
	int bytes = 0;
	memcpy(&bytes, &rawJob.GetContents()[8], sizeof(int));

	char* temp = (char*)malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		temp[i] = rawJob.GetContents()[12 + i];
	}

	data.set(temp, bytes);
}




job::job()
{
	ID = -1;
	command = Commands::None;
	data = nullptr;
}

job::job(const Buffer& rawJob)
{
	getClientID(rawJob);
	getCommand(rawJob);
	getData(rawJob);
}


job::~job()
{

}
