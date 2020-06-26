#include"connectionManager.hpp"
#include"clientManager.hpp"
#include"RDL.hpp"
#include"rdlData.hpp"
#include"dataBase.hpp"
#include<thread>
#include<mutex>

#include"customEvent.hpp"



/*Main RDL Plugin Manager functionality.*/
int exePluginManager()
{
	//clean up any mess left behind
	WSACleanup();

	//create a mutex to manage shared memory between clientManager and requestHandler
	std::mutex jobVector;

	//instantiate high level objects
	connectionManager connectionManagerObj;
	clientManager clientManagerObj;
	requestHandler reqFactory;
	RDL theRDL((const char*)"rtex10.exe");

	//start threads to get the objects "working"
	std::thread connectionManagerThread(&connectionManager::worker, &connectionManagerObj);
	std::thread clientManagerThread(&clientManager::worker, &clientManagerObj, &jobVector);
	std::thread reqFactoryThread(&requestHandler::worker, &reqFactory, &jobVector);


	//join threads on exit
	//TODO:GWC - need to somehow tell the threads to cleanly exit.
	reqFactoryThread.join();
	connectionManagerThread.join();
	clientManagerThread.join();
	return 0;
}







int main()
{
	return exePluginManager();
}