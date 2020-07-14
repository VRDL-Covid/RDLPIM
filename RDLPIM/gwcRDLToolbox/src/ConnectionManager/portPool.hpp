#pragma once
#include"connectionOptions.h"

class portStatus {
public:
	int port;
	bool status;

	portStatus() {
		port = 0;
		status = true;
	}
};

class portPool {
public:
	static portPool* instance;
	static portStatus* pool;
	static int getPort();
	static void releasePort(int p);

	 portPool();
	~portPool();
};



