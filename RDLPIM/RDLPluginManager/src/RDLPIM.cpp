#include "gwcRDLToolBox.h"


/// @brief 
/// This is the main function of the RDLPIM application, contains objecs to manage connections, manage clients, handle requests and RDL memory management.
/// 
/// @return returns a status code 
int exePluginManager()
{

PROFILE_BEGIN_SESSION("RDLPIM-Startup", "../analysis/RDLPIM-Startup.json");
	////////////////////////////////
	//Initialisation
	////////////////////////////////
	//clean up any mess left behind

	std::cout << "#######################################################" << std::endl;
	std::cout << "#                                                     #" << std::endl;
	std::cout << "#                   RDLPIM - v1.0                     #" << std::endl;
	std::cout << "#     Author:Guy Collins                              #" << std::endl;
	std::cout << "#     Date:24/08/2020                                 #" << std::endl;
	std::cout << "#                                                     #" << std::endl;
	std::cout << "#######################################################" << std::endl;
	std::cout << "                                                       " << std::endl;

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

	std::cout << "Ready for connections...                             " << std::endl;

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

int Doc()
{
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to store message for sending and recieve messages to
	Buffer recieved;

	//Helper class to build the header packet
	RequestHeader reqHeader;

	// listen for incoming data for ever!
	while (true) {
		//receive incoming from RDLPIM
		if (client.CanRead())
		{
			client.Recieve(recieved);
			//Process the header so the type of recieved can be deduced
			reqHeader.ProcessHeader(recieved);

			//if recieved chat, print the message to the console
			if (reqHeader.GetCommand() == Commands::data) {
				
				//Deserialise the data into an array of DataElements (DataElementArray)
				DataElementArray dataArray;
				dataArray.Deserialise(recieved);

				//print out name and type to console
				for (auto element : dataArray) {
					std::cout << element->GetName() << "of type " << element->GetType();
				}
			}
		}
	}
}