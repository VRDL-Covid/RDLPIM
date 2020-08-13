#pragma once
#include"client.hpp"
#include"Core/buffer.hpp"
#include"RequestHandler/requestHandler.hpp"
#include"Core/Event.h"


/// @brief A Class to manage connected Clients
/// 
/// This class manages connected clients and can be used as a single point to access and interact with all connected clients. A worker function has been provided so that the clientManager may do its job on a seperate threat.  Being a singleton, 
/// an intance can be obtained using the Get() method provided.
/// 
/// The clientManager is thread safe.
/// 
/// @author Guy Collins
/// 
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
/// 
/// @warning This class must be used in conjunction with a connectionManager
/// @warning The worker thread must be detached as it is executing a blocking socket accept, this thread can therefore not be joined.
class clientManager
{
public:
	/// @brief A method to initialise the clientManager object
	/// 
	/// This method get a reference to the connectionManager singleton and subscribes to the onNewConnection event
	/// 
	/// @warning this method must be executed before the worker method is executed.
	void Init();


	/// @brief The clientManager is a singleton, this method instantiates an instance if required and returns a raw pointer to it.
	/// @return The instance of a clientManager
	static clientManager* Get()
	{
		if (s_Instance == nullptr)
			s_Instance = new clientManager;

		return s_Instance;
	}


	/// @brief A method to send a datagram from a client of id = ID, to every other connected client
	/// @param ID ID of the source client
	/// @param output the raw datagram to be sent
	static void publishMessage(int ID, const Buffer& output);

	/// @brief A method to send a datagram from a client to every other connected client
	/// @param client reference to source client
	/// @param output output the raw datagram to be sent
	static void publishMessage(Ref<Client> client, const Buffer &output);

	/// @brief A method to send a datagram to a client of id = ID.
	/// @param ID The id of the destination client
	/// @param output The raw datagram to be sent
	static void sendMessage(int ID, const Buffer& output);

	/// @brief A method to broadcast a datagram to all connected clients
	/// @param output the raw datagram to be sent
	static void broadCast(const Buffer& output);

	/// @brief A method to register a newly connected client.
	/// 
	///  this method is invoked following the onNewConnection event raised by the connectionManager.
	/// @param newClient a Ref(shared pointer) to the newly connected client
	/// @return event propagation state
	static bool AddClient(const Ref<Client>& newClient) { return Get()->AddClient_impl(newClient); }

	/// @brief A method to unregister a connected client
	/// 
	/// This method is invoked following the onDisconnected event raised by the connectionManager.
	/// @param client a shared pointer reference to the disconnecting client
	/// @return event propagation state
	static bool RemoveClient(Ref<Client>& client) { return Get()->RemoveClient_impl(client);}



	/// @brief A background worker thread to handle the management of clients
	/// @param work a signal to drive the work, flase indicates that the thread may join.
	void worker(bool& work);

	/// @brief a getter for the onNewClient event.
	/// 
	/// This event is raised following the succeful registration of a new client
	/// @return a constant reference to the newly connected client object.
	const Event<const Ref<Client>&>& GetNewClientEvent() const { return OnNewClient; }

	/// @brief a getting for the onclientDisconnect event
	/// 
	/// This event is raised following a client disconnect.
	/// @return a const reference to the disconnected client object
	const Event<const Ref<Client>&>& GetClientDisconnectEvent() const { return OnClientDisconnect; }

	~clientManager();

private:
	static std::mutex clientDB_lock;
	static std::vector<Ref<Client>> clients;
private:

	/// @brief A method to asynchronosly check a client for any incoming datagrams.
	/// @param client The client to be tested
	/// @param output a pointer to a buffer within which to store any incoming data
	/// @return the number of bytes recieved. 0 for nothing, -1 for an invalid connection
	int checkForIncoming(Ref<Client> client, Buffer* output);

	clientManager();
	static clientManager* s_Instance;
	bool AddClient_impl(const Ref<Client>& newClient);
	bool RemoveClient_impl(Ref<Client>& client);

private://Events
	Event<const Ref<Client>&> OnClientDisconnect;
	Event<const Ref<Client>&> OnNewClient;
};

