#include "rdlpch.h"
#include "portPool.hpp"

portStatus * portPool::pool = new portStatus[MAX_CONNECTED_CLIENTS];
portPool* portPool::instance;
portPool::portPool()
{
	if (instance != this && instance != nullptr) {
		std::cerr << "portPool object already initialised";
		this->~portPool();
		return;
	}

	instance = this;

	for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
		pool[i].port = BASE_PORT + 1 + i;
		pool[i].status = true;
	}
}

int portPool::getPort()
{
	int i = 0;
	while (!portPool::pool[i].status) {
		i++;
	}
	portPool::pool[i].status = false;
	return portPool::pool[i].port;
}

void portPool::releasePort(int p)
{
	int i = 0;
	while (!(portPool::pool[i].port == p)) {
		i++;
	}

	portPool::pool[i].status = true;
}


portPool::~portPool()
{
}
