#include "gwcRDLToolBox.h"


/*Main RDL Plugin Manager functionality.*/
int exePluginManager()
{

PROFILE_BEGIN_SESSION("RDLPIM-Startup", "../analysis/RDLPIM-Startup.json");
	////////////////////////////////
	//Initialisation
	////////////////////////////////
	//clean up any mess left behind
	WSACleanup();

	//Thread state Variable
	bool work = true;
	char exit = '-';

	//instantiate high level objects
	connectionManager* connectionManagerObj = connectionManager::Get();
	clientManager* clientManagerObj = clientManager::Get();
	requestHandler* reqFactory = requestHandler::Get();
	RDL* theRDL = RDL::Get();

	//initialise Objects that need to be.
	connectionManagerObj->Init();
	clientManagerObj->Init();
	theRDL->Init("rtex10.exe");
PROFILE_END_SESSION();


PROFILE_BEGIN_SESSION("RDLPIM-Runtime", "../analysis/RDLPIM-Runtime.json");
	////////////////////////////////
	// Runtime execution
	////////////////////////////////
	//start threads to get the objects "working"
	std::thread rdlManager(&RDL::worker, theRDL, std::ref(work));
	std::thread connectionManagerThread(&connectionManager::worker, connectionManagerObj, std::ref(work));
	std::thread clientManagerThread(&clientManager::worker, clientManagerObj, std::ref(work));
	std::thread reqFactoryThread(&requestHandler::worker, reqFactory, std::ref(work));

	while (exit != '!') {
		std::cin >> exit;
	}
	work = false;
PROFILE_END_SESSION();


	////////////////////////////////
	// Terminate execution
	////////////////////////////////
	rdlManager.join();
	reqFactoryThread.join();
	clientManagerThread.join();
	connectionManagerThread.detach();

	return 0;
}



int main()
{
	return exePluginManager();
}