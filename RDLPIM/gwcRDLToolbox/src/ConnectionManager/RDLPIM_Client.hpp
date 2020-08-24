#pragma once
#include"ConnectionManager/socketConnection.hpp"

/// @brief A class to handle a client connection to the RDLPIM
/// 
/// This class provides a method to get connection details And use them to complete the handshake with the RDLPIM (connectToRDLPIM())
/// 
/// ###Example
/// 
/// ~~~~~~~~~~~.cpp
/// rdlpimClient client1("127.0.0.1", 8000);
/// client1.connectToRDLPIM();
/// Buffer recieved;
///
///
/// bool SomethingToSend;
/// while (true) {
///
///		if (client1.CanRead()) {
///			client1.Recieve(&recieved);
///		}
///
///		if (SomethingToSend && client1.CanSend()
///		{
///			client1.Send("some message");
///		}
/// }
/// ~~~~~~~~~~~

class rdlpimClient
{
public:


	/// @brief A method to make a connection attempt to the RDLPIM and deserialise the connectiondetails and complete the handshake
	void connectToRDLPIM();

	rdlpimClient();

	/// @brief Returns true if the endpoint is trying to send this client a message
	/// @return can read state
	bool CanRead();

	/// @brief Returns true if the endpoint is available to recieve a request
	/// @return ready to send state
	bool CanSend();

	/// @brief A method to recieve messages from the RDLPIM
	/// @param buff pointer to a buffer to store the response in
	/// @return number of bytes recieved
	int Recieve(Buffer* buff);


	/// @brief A method to send data such as a request packet to the RDLPIM
	/// @param buff A reference to the data to be sent contained within a Buffer object
	/// @return number of bytes sent
	int Send(const Buffer& buff);

	rdlpimClient(const char* IP, int port);
	~rdlpimClient();

private:
	connectionObject connection;
	int port;
	int ID;

};

