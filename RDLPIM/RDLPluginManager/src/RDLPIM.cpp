#include "gwcRDLToolBox.h"
/*Main RDL Plugin Manager functionality.*/
int exePluginManager()
{
PROFILE_BEGIN_SESSION("RDLPIM-Startup", "../analysis/RDLPIM-Startup.json");
	//clean up any mess left behind
	WSACleanup();

	//Thread state Variable
	bool work = true;
	char exit = 'a';
	//create a mutex to manage shared memory between clientManager and requestHandler
	std::mutex jobVectorMutex;

	//instantiate high level objects
	connectionManager* connectionManagerObj = connectionManager::GetInstance();
	clientManager* clientManagerObj = clientManager::GetInstance();
	requestHandler* reqFactory = requestHandler::GetInstance();
	//RDL theRDL((const char*)"rtex10.exe");

	//initialise Object
	connectionManagerObj->Init();
	clientManagerObj->Init();
PROFILE_END_SESSION();

PROFILE_BEGIN_SESSION("RDLPIM-Runtime", "../analysis/RDLPIM-Runtime.json");
	//start threads to get the objects "working"
	std::thread connectionManagerThread(&connectionManager::worker, connectionManagerObj, std::ref(work));
	std::thread clientManagerThread(&clientManager::worker, clientManagerObj, std::ref(work) ,&jobVectorMutex);
	std::thread reqFactoryThread(&requestHandler::worker, reqFactory, std::ref(work) ,&jobVectorMutex);

	while (exit != '!') {
		std::cin >> exit;
	}

	work = false;
	//join threads on exit
	//TODO:GWC - need to somehow tell the threads to cleanly exit.
PROFILE_END_SESSION();
	reqFactoryThread.join();
	clientManagerThread.join();
	connectionManagerThread.join();

	return 0;
}



int main()
{
	return exePluginManager();
}