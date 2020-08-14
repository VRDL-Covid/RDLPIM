#pragma once
#include"connectionOptions.h"
#include"socketConnection.hpp"
#include"portPool.hpp"
#include"Core/buffer.hpp"
#include"ClientManager/clientManager.hpp"

/// @brief The connectionManager class handles incoming connection request attempts and manages the hand shake
/// 
/// A client make a connect request on a blocking socket on port 8000. The client simply has to bind to this port and the
/// RDLPIM will send connection details (port and a client ID) should a port be availble. The client can then reconnect on the provided port
/// using an asynchronous socket.  This will be the highway for data into and out of the RDLPIM. The connectionManager follows the singleton design pattern
/// an instance can be instantiated and obtained using the Get() method provided.
/// 
/// @author Guy Collins
/// 
/// @warning The connectionManager must be used in conjunction with a clientManager.
/// ###Example
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// bool work = true;
/// 
/// connectionManager* connectionManagerObj = connectionManager::Get();
/// clientManager* clientManagerObj = clientManager::Get();
/// 
/// connectionManagerObj->Init();
///	clientManagerObj->Init();
/// 
///	std::thread connectionManagerThread(&connectionManager::worker, connectionManagerObj, std::ref(work));
/// std::thread clientManagerThread(&clientManager::worker, clientManagerObj, std::ref(work));
/// 
/// clientManagerThread.join();
/// connectionManagerThread.detach();
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class connectionManager
{
public:
	/// @brief A method to initialise the connectionManager object
	/// 
	/// This method get a reference to the clientManager singleton and subscribes to the ClientDisconnectEvent event
	/// 
	/// @warning this method must be executed before the worker method is executed.
	void Init();


	/// @brief This method tidies up the connection associated with a disconnecting client
	/// 
	/// The client socket is closed and the port is released back into the available ports pool
	/// @param user A refernce to the disconnecting client
	/// @return event propogation state
	bool ClientDisconnectCallback(const Ref<Client>& user);

	/// @brief A background worker thread to handle the management of client connection objects
	/// 
	/// listens for incoming connection requests, serves connection data and handles the handshake.
	/// @param work a signal to drive the work, flase indicates that the thread may join.
	void worker(bool& work);

	~connectionManager();

	/// @brief Instantiates and returns a singleton instance of a connectionManager.
	/// @return raw pointer to a connectionManager object.
	static connectionManager* Get()
	{
		if (s_Instance == nullptr)
			s_Instance = new connectionManager;
		return s_Instance;
	}

	/// @brief A getter to the onNewConnection event
	/// 
	/// This event is raised following the succesfull connection and handshake of a new connection.
	/// @return 
	const Event<const Ref<Client>>& GetNewConnectionEvent() const { return onNewConnection; }

private:
	static connectionManager* s_Instance;
	
	Event<const Ref<Client>> onNewConnection;
	connectionObject listener;
	portPool ports;
	Ref<Client> processingClient;

	connectionManager();
	void connectToClient();
	void buildConnectionDetails(Buffer* connectionReq);
	void sendConnectionDetails();


};