#include "rdlpch.h"
#include "job.hpp"

commands job::getCommand(buffer* rawJob)
{
	int position = 0;
	commands Tcommand = commands::INVALID;

	while ((position < rawJob->size) && !(((rawJob->contents[position + 0] == 'C') || (rawJob->contents[position + 0] == 'c'))
		&& ((rawJob->contents[position + 1] == 'M') || (rawJob->contents[position + 1] == 'M'))
		&& ((rawJob->contents[position + 2] == 'D') || (rawJob->contents[position + 2] == 'D'))
		&& (rawJob->contents[position + 3] == '='))) {
		position++;
	}
	position += sizeof("CMD");;

	if (position >= rawJob->size) {
		command = commands::INVALID;
		return command;
	}

	memcpy(&Tcommand, &rawJob->contents[position], sizeof(Tcommand));
	command = Tcommand;
	return command;

}

int job::getClientID(buffer* rawJob)
{
	int position = 0;
	int tempID = 0;
	while ((position < rawJob->size) && !(((rawJob->contents[position + 0] == 'I') || (rawJob->contents[position + 0] == 'i'))
									   && ((rawJob->contents[position + 1] == 'D') || (rawJob->contents[position + 1] == 'd'))
									   && (rawJob->contents[position + 2] == '='))) {
		position++;
	}
	position += sizeof("ID");



	if (position >= rawJob->size) {
		ID = -1;
	}
	else {
		memcpy(&tempID, &rawJob->contents[position], sizeof(int));
		ID = tempID;
	}

	return ID;
}

void job::getData(buffer* rawJob)
{
	int position = 0;
	int bytes = 0;
	while ((position  < rawJob->size) && !(((rawJob->contents[position + 0] == 'D') || (rawJob->contents[position + 0] == 'd'))
										   && ((rawJob->contents[position + 1] == 'A') || (rawJob->contents[position + 1] == 'a'))
										   && ((rawJob->contents[position + 2] == 'T') || (rawJob->contents[position + 2] == 't'))
										   && ((rawJob->contents[position + 3] == 'A') || (rawJob->contents[position + 3] == 'a'))
										   && (rawJob->contents[position + 4] == '='))) {
		position++;
	}
	position += sizeof("DATA");
	memcpy(&bytes, &(rawJob->contents[position]), 4);
	position += sizeof(int);
	
	char* temp = (char*)malloc(bytes);

	for (int i = 0; i < bytes; i++) {
		temp[i] = rawJob->contents[position + i];
	}

	data.set(temp, bytes);
}




job::job()
{
	ID = -1;
	command = commands::INVALID;
	data = nullptr;
}

job::job(buffer* rawJob)
{
	getClientID(rawJob);
	getCommand(rawJob);
	getData(rawJob);
}


job::~job()
{

}
