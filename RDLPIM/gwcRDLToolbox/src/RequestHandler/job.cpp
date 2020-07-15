#include "rdlpch.h"
#include "job.hpp"

Commands job::getCommand(Buffer* rawJob)
{
	memcpy(&command, &rawJob->contents[4], sizeof(int));
	return command;
}

int job::getClientID(Buffer* rawJob)
{
	memcpy(&ID, &rawJob->contents[0], sizeof(int));
	return ID;
}

void job::getData(Buffer* rawJob)
{
	int bytes = 0;
	memcpy(&bytes, &rawJob->contents[8], sizeof(int));

	char* temp = (char*)malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		temp[i] = rawJob->contents[12 + i];
	}

	data.set(temp, bytes);
}




job::job()
{
	ID = -1;
	command = Commands::None;
	data = nullptr;
}

job::job(Buffer* rawJob)
{
	getClientID(rawJob);
	getCommand(rawJob);
	getData(rawJob);
}


job::~job()
{

}
