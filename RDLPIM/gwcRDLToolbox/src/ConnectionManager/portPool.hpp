#pragma once
#include"connectionOptions.h"

/// @brief A class to store the staus of ports within the port pool
/// 
/// @author Guy Collins
class portStatus {
public:
	int port; ///< port number
	bool status; ///< the ports status, true is available. false is used

	/// @brief Constructor
	/// 
	/// Initialises port number to 0 and status available(true)
	portStatus() {
		port = 0;
		status = true;
	}
};

/// @brief The port pool allows available ports to be served and keeps track of currently used ports.
class portPool {
public:
	static portPool* instance; ///<instance of port pool
	static portStatus* pool; ///<Pointer to an array of ports, MAX_CONNECTED_CLIENTS in number

	/// @brief Method to return next available port
	/// @return port number
	static int getPort();

	/// @brief Method to release a port back into the pool
	/// @param p port number to be released
	static void releasePort(int p);

	 portPool();
	~portPool();
};



