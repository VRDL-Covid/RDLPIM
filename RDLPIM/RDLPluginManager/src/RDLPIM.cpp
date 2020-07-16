#include "gwcRDLToolBox.h"
/*Main RDL Plugin Manager functionality.*/
int exePluginManager()
{
	//clean up any mess left behind
	WSACleanup();

	//create a mutex to manage shared memory between clientManager and requestHandler
	std::mutex jobVectorMutex;

	//instantiate high level objects
	connectionManager* connectionManagerObj = connectionManager::GetInstance();
	clientManager* clientManagerObj = clientManager::GetInstance();
	requestHandler* reqFactory = requestHandler::GetInstance();
	RDL theRDL((const char*)"rtex10.exe");

	//initialise Object
	connectionManagerObj->Init();
	clientManagerObj->Init();

	//start threads to get the objects "working"
	std::thread connectionManagerThread(&connectionManager::worker, connectionManagerObj);
	std::thread clientManagerThread(&clientManager::worker, clientManagerObj, &jobVectorMutex);
	std::thread reqFactoryThread(&requestHandler::worker, reqFactory, &jobVectorMutex);


	//join threads on exit
	//TODO:GWC - need to somehow tell the threads to cleanly exit.
	reqFactoryThread.join();
	clientManagerThread.join();
	connectionManagerThread.join();
	return 0;
}



int main()
{
	return exePluginManager();
}