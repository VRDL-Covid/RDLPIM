#include "rdlpch.h"
#include "dataArray.hpp"



void dataArray::deserialise(const buffer& rawInput)
{
	buffer singleReqSerialised;
	buffer rawInputCpy = rawInput;

	int it = 0;

	char* temp = nullptr;

	while (rawInputCpy.size > 0) {
		responceElement* nextReq = new responceElement;

		//consume name
		while (rawInputCpy.contents[it] != '=') {
			it++;
		}

		temp = (char*)realloc(temp, it);

		for (int i = 0; i < it; i++) {
			temp[i] = rawInputCpy.contents[i];
		}
		

		nextReq->varName.set(temp, it);
		it = 0;

		rawInputCpy.stripHead('=');

		//consume type
		while (rawInputCpy.contents[it] != '=') {
			it++;
		}

		temp = (char*)realloc(temp, it);

		for (int i = 0; i < it; i++) {
			temp[i] = rawInputCpy.contents[i];
		}

		buffer Ttype;
		Ttype.set(temp, it);
		nextReq->type = Ttype;
		it = 0;

		rawInputCpy.stripHead('=');

		memcpy(&(nextReq->bytes), rawInputCpy.contents, sizeof(int));
		if (nextReq->bytes > 0 ) {
			temp = (char*)realloc(temp, nextReq->bytes);
			memcpy(temp, &(rawInputCpy.contents[sizeof(int)]), nextReq->bytes);
			nextReq->set(temp, nextReq->bytes);
			nextReq->type = Ttype;
			rawInputCpy.stripHead((int)(sizeof(int) + (nextReq->bytes)));

		}

		if (!strncmp(nextReq->type.contents, "ERR-NFND", (sizeof("ERR-NFND")-1))) {
			rawInputCpy.stripHead((int)sizeof(int));
		}

		data.push_back(nextReq);
		nElements++;
	}
	
	free(temp);

	
	return;
}

dataArray::dataArray()
{
	nElements = 0;
}


dataArray::~dataArray()
{
	for (int i = 0; i < nElements; i++) {
		delete data[i];
	}
}
