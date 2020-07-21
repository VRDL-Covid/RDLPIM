#include "gwcRDLToolBox.h"
/*Main RDL Plugin Manager functionality.*/
int exePluginManager()
{
	////////////////////////////////
	//Initialisation
	////////////////////////////////
PROFILE_BEGIN_SESSION("RDLPIM-Startup", "../analysis/RDLPIM-Startup.json");
	//clean up any mess left behind
	WSACleanup();

	//Thread state Variable
	bool work = true;
	char exit = 'a';

	//todo GWC - this mutex should be inside the reqFactory object, it shouldnt need to be created at this level.
	//create a mutex to manage shared memory between clientManager and requestHandler
	std::mutex jobVectorMutex;

	//instantiate high level objects
	connectionManager* connectionManagerObj = connectionManager::Get();
	clientManager* clientManagerObj = clientManager::Get();
	requestHandler* reqFactory = requestHandler::Get();
	RDL* theRDL = RDL::Get();

	//initialise Object
	connectionManagerObj->Init();
	clientManagerObj->Init();
	theRDL->Init("rtex10.exe");

PROFILE_END_SESSION();


	////////////////////////////////
	// Runtime execution
	////////////////////////////////


PROFILE_BEGIN_SESSION("RDLPIM-Runtime", "../analysis/RDLPIM-Runtime.json");
	//start threads to get the objects "working"
	std::thread connectionManagerThread(&connectionManager::worker, connectionManagerObj, std::ref(work));
	std::thread clientManagerThread(&clientManager::worker, clientManagerObj, std::ref(work) ,std::ref(jobVectorMutex));
	std::thread reqFactoryThread(&requestHandler::worker, reqFactory, std::ref(work) ,std::ref(jobVectorMutex));

	while (exit != '!') {
		std::cin >> exit;
	}

	work = false;
	//join threads on exit
	//TODO:GWC - need to somehow tell the threads to cleanly exit.
PROFILE_END_SESSION();


	reqFactoryThread.join();
	clientManagerThread.join();
	//TODO:GWC  Connection Manager get next connection needs to be non-blocking, thread cant exit.
	connectionManagerThread.join();

	return 0;
}



int main()
{
	return exePluginManager();
}