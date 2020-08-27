#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <queue>
#include <deque>
#include <bitset>
#include <utility>
#include <algorithm>
#include <functional>

#include"Core/Core.h"

//ifDebug
#include"Debugging/Profiling/Instrumentor.h"




/*! \mainpage RDLPIM User Guide
 *
 * \section intro_sec Introduction
 * 
 * ###What is the RDLPIM?

* The RDLPIM (Reactor Display Laboratory Plug-In Manager) is a server that runs in parallel to the RDL, allowing safe access to the RDLs process memory at runtime via its \ref APIUG "API".  
* Although the RDLPIMs primary function is to facilitate interaction with the RDL via external sources, the RDLPIM acts as an intermediary for generic data exchange between any number of sources.
* For example Uinty projects to c#, c++ code and/or/without the RDL.
* 
* \htmlonly <style>div.image img[src="HLOV.png"]{width:60%;}</style> \endhtmlonly 
* \image html HLOV.png "Figure 1 - High Level Overview of Runtime Architecture" 
* 
* For information on how to connect to and exploit the RDLPIM please see \ref APIUG "API User Guide Page"
 *
 * Source files for the RDLPIM (V1.0) can be downloaded <a href="source.zip">here</a> 
 * 
 * Source files to an example Unity Client can be downloaded from here <a href="unityClient.zip">here</a> 
 * 
 * For an updated relese please see the RDLPIMs github page <a href="https://github.com/VRDL-Covid/RDLPIM">here</a>
 * 
 * For any questions regarding the code, please contact the VRDL technical lead and author of this code, Guy Collins at 
 * 
 * Email: Guy.Collins@Rolls-Royce.com
 * 
 * Email-alt gwc593@gmail.com
 * 
 * Phone: 07534262038
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * \section install_sec Compilation and Installation
 *
 * \subsection step1 Step 1: Obtaining the Source Files
 *
 * Source files for the RDLPIM (V1.0) can be downloaded <a href="source.zip">here</a>
 * 
 * For an updated relese please see the RDLPIMs github page <a href="https://github.com/VRDL-Covid/RDLPIM">here</a>
 * 
 * \subsection buildProjects Step2 : Building the Projects
 * To Date Windows is the only operating system supported by the RDLPIM.  This section describes how to build visual studio project files (upto and including VS2019) in order to easily build and debug the RDLPIM.
 * A test harnes project is also built inorder to facilitate debugging and testing of the RDLPIM.
 * 
 * Once you have downloaded and unzipped the contents of <a href="source.zip">source.zp</a> right click and edit the "generateProjects.bat".  This batchfile is pre configured to build Visual Studio 2019 project
 * files. If you require a different version of visual studio please change 'vs2019' to vs2017 for Visual Studio 2017 and so on.  Supported IDEs are
 * 
 * IDE Code | IDE | RDLPIM Supported
 * ------------- | -------------|:-------------:
 * vs2019	|Generate Visual Studio 2019 project files | Yes
 * vs2017	|Generate Visual Studio 2017 project files | Yes
 * vs2015	|Generate Visual Studio 2015 project files | Yes
 * vs2013	|Generate Visual Studio 2013 project files | Yes
 * vs2012	|Generate Visual Studio 2012 project files | Yes
 * vs2010	|Generate Visual Studio 2010 project files | Yes
 * vs2008	|Generate Visual Studio 2008 project files | Yes
 * vs2005	|Generate Visual Studio 2005 project files | Yes
 * gmake	|Generate GNU Makefiles (This generator is deprecated by gmake2) | No
 * gmake2	|Generate GNU Makefiles (including Cygwin and MinGW) | No
 * xcode4	|XCode projects (built-in extension) | No
 * codelite	|CodeLite projects (built-in extension) | No
 * 
 * Once the "generateProjects.bat" has been configured, execute it by double clicking it. A visual studio solution file should now exist next to the "generateProjects.bat" file. Double click this to open.
 * 
 * 
 * 
 * 
 * \subsection compiling Step3 : Compiling
 * 
 * The visual studio has several solution configurations to compile the RDLPIM for either the end user or developer.  The table below outlines the available configurations and who they are intended for.
 *  
 * Solution Configuration | Overview 
 * ------------- | :-------------:
 * Debug |For developers. Produces an unoptimised binary with debug symbols available. Use this to test your modifications to the code.
 * Dist	|For end user deployment.  Produces an optimised binary with the symbols stripped for the code. Fastest execution.
 * Profiling |For developers. Produces an unoptimised binary with debug symbols available. Generates JSON formated timing information of the internal functions of the Code see \ref profiling "Profiling" for more information
 * Release |For developers.  Produces an optimised binary with the symbols still included. This should be executed to check that compiler optimisation does not result in unexpected behaviour.
 * 
 * 
 * Once you have selected the desired solution configuration, press Ctrl+Shift+B or go to Build-> Build Solution in order to build the binaries.
 * 
 * 
 * 
 * 
 * \subsection running Step3 : Execution
 * 
 * The RDLPIM requires access to DLLs provided by the GSE suite inorder to determine the runtime location of variables within the RDL.  In order for the RDLPIM to be executed, a GSE "S:" drive will need to be
 * mapped and the RDLPIM binary launched via a CMDLOAD console.  This is required to ensure the correct environmental parameters are set and DLL access is available. 
 * 
 * At this stage you will be greeted with a welcome message deatiling the version of the RDLPIM being executed and its status.
 * 
 * \warning
 * For convenience and home testing during the Covid crisis, the required DLLs will be copied next to the binary upon building, for onsite use, delete these DLLs to enure the correct ones are referenced via the S: drive.
 */



 /*! \page APIUG Client API User's Guide

 \par Table of Contents

 - \ref APIUG
    - \ref apiIntro "API Introduction"
    - \ref apiConnection "API Connection and Handshake"
    - \ref APICommands "Client Side Request Commands"
        - \ref apiPull "Pull Data"
        - \ref apiPush "Push Data"
        - \ref apiSubscribe "Subscribe Data"
        - \ref apiUnsubscribe "Unsubscribe Data"
        - \ref apiChat "ASCII Chat"
    - \ref apiRecieved "Client Side Recieved Commands"
        - \ref apiData "Received Data"
        - \ref apiInfo "Server Info"
        - \ref apiError "Error Packets"




\subsection apiIntro API Introduction
    Connections and requests made to the RDLPIM are done so via the RDLPIM's custom API.  This section of the documentation provides a detailed description of how to build connection and request packets, as well as
    pointing to helper classes which will make this easier for a developer developing a client.

\subsection apiConnection Requst Connection and Handshake

The RDLPIM exposes a synchronous/blocking TCP listener socket on port 8000.  Once a client makes a connection to this socket, the RDLPIM will serve the client with connection details and close the connection.
The format of the connection details packet is as follows

[Function Word]=[Port]=[Client ID]

[**char\***]=        [**int**]  =        [**int**]

CONNECT=0x41 0x1f 0x00 0x00=0xe8 0x03 0x00 0x00

###Deserialised:

Function = Connect

Port = 8001

ClientID = 1000


The RDLPIM will then open a listening asynchronous TCP socket of the specified port for the client to reconnect and bind to.  A helper class has been provided for c++ application (rdlpimClient) an example of its initialisation has been provided below.

####Example

~~~~~~~~~~~~.cpp
    rdlpimClient client1("127.0.0.1", 8000);
    client1.connectToRDLPIM();
    Buffer recieved;


    bool SomethingToSend;
    while (true) {

		if (client1.CanRead()) {
            client1.Recieve(recieved);
        }

        if(SomethingToSend && client1.CanSend(){
            client1.Send("some message");
        }
    }
~~~~~~~~~~~~


\subsection APICommands Client Side Request Commands

A client is able to Push, Pull, Subscribe and Unsubscribe from data stored within the RDLPIM/RDL,  The client does not need to make a destinction between whether the data requested exists on the RDLPIM or RDL runtime, this is handled by the RDLPIM. 
In addition to the data manipulating requests, function codes denoting a data packet, server info and inter-client chat also exist.  
This section of the documentation attempts to explain in detail how to build these packets from the ground up, and whathelper classes exist to help you serialise and deserialise messages to and from the RDLPIM.

All requests made to the RDLPIM are done so via an Asynchronous TCP socket, negotiatied following \ref apiConnection "connection with the RDLPIM".  
Request packets start with an 8byte header, the first 4 bytes are the int32 represnetation of the \ref Commands "function code" for example pull = 4 = 0x04 0x00 0x00 0x00.  
The next 4 bytes is the int32 representation of the length in bytes of the remaining data within the request packet. Please see below for detailed examples of how to build request packets and how to deserialise their result.

\subsubsection apiPull API Command - Pull Data

The Pull data request code is used to Pull data from the RDLPIM, the value of the pull request code as of RDLPIM V1.0 is (Int32)4 (0x04 0x00 0x00 0x00) and forms the first 4 bytes of the request header packer. The next 4 bytes is the int32 representation of the length in bytes of the request data.
The request data contains the variable names you wish to pull from the RDLPIM, multiple variables can be stacked by encapsulating them with curly braces, curly braces are therefore illegal characters 
for a variable name

The Table below shows a simple pull request architecture in raw bytes.

####Example Pull Request Packet
 * byte | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11 | 12| 13| 14| 15| 16| 17| 18| 19| 20
 * -----|:----:|----|----|----|:----:|----|----|----|:---:|---|---|---|---|---|---|---|---|---|---|---|---
 * **Description**|Function Code||||Size of Request Data||||Request Data|||||||||||||
 * **Hex/Char**|0x04|0x00|0x00|0x00|0x0D|0x00|0x00|0x00|'{'|'t'|'h'|'_'|'t'|'e'|'m'|'p'|'_'|'e'|'f'|'f'|'}'
 * **Human Readable**|4||||13||||{th_temp_eff}|||||||||||||
 * **Length(bytes)**|4||||4||||13|||||||||||||


\warning **1)** Function code/command code values are likely to change through time as the RDLPIM is extended. It is therefore recommended that the enumerator class \ref Commands is exported to generate concurant request codes ed Commands::pull. 


\warning **2)** '{' and '}' are illegal characters for a variable name

Upon recieveing a pull request the RDLPIM will preferentially look for the data requested within its internal DataBase, failing this, it will check the RDLs runtime memory. 

The return to the client is a \ref apiData "Data" packet containing an array of serialised 
\ref DataElement "DataElements", this can be deserialised using the DataElemenArray helper class in c++ or within c# (see <a href="unityClient.zip">Example Unity Project.</a> ) DataElement.DeserialiseArray((byte[])rawData) to produce a List<DataElement>.
If the RDLPIM is unable to find the request a DataElement will be returned in a \ref apiData "Data" packet of type 'ERR-NFND'.  
If the request the RDLPIM will return a message with a header command code of Commands::error (int32 9 as of RDLPIM v1.0)

Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to build a pull request and deserialise the recieve.

#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h


int main(){
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to store request packet for sending
	Buffer requestPacket;

	//Buffer to store the return from RDLPIM
	Buffer responce;

	//Helper class to build the header packet
	RequestHeader pullReq;

	//a DataElement to store the data returned from the RDLPIM
	DataElement reqVariable("th_temp_eff");

	//Array of DataElements to be recieved from the RDLPIM
	DataElementArray dataRecieved;

	//build the header packet
	//set the command
	pullReq.SetCommand(Commands::pull);
	//set the data size
	requestPacket = pullReq.SetData(reqVariable.SerialiseName());


	//Send the request
	if (client.CanSend())
	{
		client.Send(requestPacket);
	}

	//Recieve the return
	while (responce.GetSize() < 1) {
		if (client.CanRead()) {
			client.Recieve(responce);
		}
	}

	//Deserialse the header and strip it from the responce so you are left with the raw data
	pullReq.ProcessHeader(responce);

	//if recieved data from RDLPIM, deserialise it
	if (pullReq.GetCommand() == Commands::data) {
		//deserialise data to a DataElement so its size and data are easily accessable.
		dataRecieved.Deserialise(responce);
	}

	//print out variables recieved and their types
	for(auto element : dataRecieved){
		std::cout << element.GetName() << "is type " << element.GetType() << std::endl;
	}
}

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class PullRequest{

	//get instance to the provided RDLPIM interface singleton
    private RDLPIM_Controller rdlController;

	//Container to store recieved RDLPIM data within
	public List<DataElement> dataRecieved;


	//Constructor to initialise the reference to the RDLPIM interface
	// and subscribe to the event raised upon recieving new data. This event passes the data recieved in the form
	// of a list of DataElements
    PullRequest()
    {
        rdlController = RDLPIM_Controller.Instance;
        rdlController.DataRecieved += onNewData;
    }

	//It is important to unsubscribe call back on the event the object get destroyed (scene change or object deleted from scene) failure to do so will
	//raise errors at runtime and reduce frame rate
	public void OnDestroy
	{
		rdlController.DataRecieved -= onNewData;
	}


	//Method to send multiple pull requests, 
	//variables names are passed as a list of strings 
	public void SendPulls(List<string> vars)
	{
		List<DataElement> reqs = new List<DataElement>();
		foreach(string varname in vars){
			reqs.Add(new DataElement(varname));
		}
		
		rdlController.SendPull(reqs);
	}


	//Callback for the onDataRecieved event on the RDLPIM interface class (RDLPIM_Controller)
	// This method simply copies the received data into this instqance of the PullRequest class
	protected void onNewData(object source, List<DataElement> newData)
    {
		dataRecieved = newData;
    }
}
~~~~~~~~~~~~~~~~~~~~~~~





\subsubsection apiPush API Command - Push Data
The Push data request code is used to push data into the RDLPIM.  Upon recipt of a Push data request, the RDLPIM will firstly check to 
see if a reference to the data exists within the RDLs runtime memory.  If it does, the data will be safely pushed into the RDL at runtime
allowing a client to interact with the simulation.  If a reference does not exist within the RDL, the data will be stored in the DataBase internal
to the RDLPIM. 

\warning There is no responce from the RDLPIM to say whether the push was successful or not.  This is an intentional design decision made
to enure that the execution is as rapid as possible.  If this is an issue for the end user, please raise it as an issue on the 
<a href="https://github.com/VRDL-Covid/RDLPIM/issues">GitHub issues page.</a> 

The first 4 bytes of the request form the \ref Commands "function code" for a Push, as of RDLPIM v1.0 this is (Int32) 3 (0x03 0x00 0x00 0x00).  
The next 4 bytes is the int32 representation of the length in bytes of the data to be pushed.  The data passed with a push request is a series of
serialised DataElements. Lets consider a simple example of pushing a variable called 'pi', of type 'Double', and value 3.14159. This DataElement would
serialise to:



####Serialised DataElement
 * byte        | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11  |  12|  13|  14|  15|  16|  17|  18|  19|  20|  21|  22|  23
 * -----       |:----:|:----:|----|:----:|:----:|----|----|----|---|---|:---:|:----:|----|----|----|:----:|----|----|----|----|----|----|----|:----:
 * **Description**|Start DataElement Delimiter|VarName||Varname Delimiter|Type||||||Type Delimiter|DataSize||||Data in Binary||||||||End DataElement Delimiter|
 * **Hex/Char**|'{' |'p' |'i' |'=' |'d' |'o' |'u' |'b' |'l'|'e'|'='|0x08|0x00|0x00|0x00|0x40|0x09|0x21|0xF9|0xF0|0x1B|0x86|0x6E|'}'
 * **Human Readable**|{|pi||=|double||||||=|8||||(double)3.14159||||||||}|


 To push this data into the RDLPIM, the request header is appended to the above before sending to the RDLPIM, the full request packet in this instance would 
 look as follows:

 ####Example Push Request Packet
 * byte       | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11  |  12|  13|  14|  15|  16|  17|  18|  19|  20|  21|  22|  23|  24|  25|  26|  27|  28|  29|  30|  31
 *        -----|:----:|:----:|:----:|:----:|:----:|----|:----:|:----:|:---:|---|:---:|:----:|----|----|----|:----:|----|----|----|----|----|----|----|:----:|----|----|----|----|----|----|----|----
 * **Description**|Function Code||||Size of Request Data||||Request Data||||||||||||||||||||||||
 * **Hex/Char**|0x03|0x00|0x00|0x00|0x18|0x00|0x00|0x00|'{' |'p' |'i' |'=' |'d' |'o' |'u' |'b' |'l'|'e'|'='|0x08|0x00|0x00|0x00|0x40|0x09|0x21|0xF9|0xF0|0x1B|0x86|0x6E|'}'
 * **Human Readable**|3||||24||||DataElement Serialised||||||||||||||||||||||||
 * **Length bytes**|4||||4||||24||||||||||||||||||||||||

 Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to build push requests and send them to the RDLPIM.

#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h


int main(){
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to contain the push request data
	Buffer requestData;

	//Build DataElement
	DataElement dataElement("pi");
	dataElement.set((double)3.14159);

	//serialise Request Data
	requestData.append(dataElement.Serialise());

	//Build the header Packet
	RequestHeader req;
	req.SetCommand(Commands::push);

	//Add the header to the request data
	requestData = req.SetData(requestData);

	//send the push request
	if (client.CanSend()) {
		client.Send(requestData);
	}
}

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class PushRequest{

	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController;

	//Constructor to initialise the reference to the RDLPIM interface
	PushRequest()
	{
		rdlController = RDLPIM_Controller.Instance;
	}


	//Function to send DataElement containing pi, multiple DataElements could be added to 'reqs' for sending
	public void SendPi()
	{
		//Array/List of DataElements for pushing
		List<DataElement> reqs = new List<DataElement>();

		//Construct the data to be sent
		DataElement dataElement = new DataElement(item.GetVarname('pi'));;
		dataElement.Set((double)3.14159);

		//Add DataElement to send array
		reqs.Add(dataElement);

		//Send the push request to the RDLPIM
		rdlController.SendPush(reqs);
	}
}
~~~~~~~~~~~~~~~~~~~~~~~




\subsubsection apiSubscribe API Command - Subscribe Data
The Subscribe data request code is used to passively recieve data from the RDLPIM once it has changed. Upon recieving a Subscribe data request, the RDLPIM will initialy try to discover this data from within its own DataBase, if this fails it will look for it within the RDL.  
If the Data exists within either, the client will be served the data at the time of making the subscribe request via a \ref apiData "Data packet".  Subsequently, any change detected to a subscibed data source will trigger updates to be passively sent to the client without the
need for polling.  If a requested variable is not found, a DataElement will be served to the client of type 'ERR-NFND', the client will however be subscribed to this variable should a source become available at a later time.  To unsubscribe from a data request, please supply
the RDLPIM with an \ref apiUnsubscribe "unsubscribe" request packet.

The first 4 bytes of a Subscribe request is the value of the subscribe request code, as of RDLPIM V1.0 is (Int32)5 (0x05 0x00 0x00 0x00). The next 4 bytes is the int32 representation of the length in bytes of the request data. For a subscribe request packet. 
The request data contains the variable names you wish to subscribe to from the RDLPIM, multiple variables can be stacked by encapsulating them with curly braces, curly braces are therefore illegal characters for a variable name

\warning Data subscribed to that exists within the RDL will be scanned for changes at a rate of 10Hz.  An improvement has been logged within the <a href="https://github.com/VRDL-Covid/RDLPIM/issues">RDLPIM's GitHub issues page.</a>, if you require a different rate or a variable rate
please contact the lead developer (Guy Collins [see \ref RDLPIM "main page" for contact details]).


 The Table below shows a simple subscribe request architecture in raw bytes.

####Example Subscribe Request Packet
 * byte | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11 | 12| 13| 14| 15| 16| 17| 18| 19| 20
 * -----|:----:|----|----|----|:----:|----|----|----|:---:|---|---|---|---|---|---|---|---|---|---|---|---
 * **Description**|Function Code||||Size of Request Data||||Request Data|||||||||||||
 * **Hex/Char**|0x05|0x00|0x00|0x00|0x0D|0x00|0x00|0x00|'{'|'t'|'h'|'_'|'t'|'e'|'m'|'p'|'_'|'e'|'f'|'f'|'}'
 * **Human Readable**|5||||13||||{th_temp_eff}|||||||||||||
 * **Length(bytes)**|4||||4||||13|||||||||||||


\warning **1)** Function code/command code values are likely to change through time as the RDLPIM is extended. It is therefore recommended that the enumerator class \ref Commands is exported to generate concurant request codes ed Commands::subscribe. 


\warning **2)** '{' and '}' are illegal characters for a variable name



Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to build a subscribe request and deserialise the recieved data.

#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h


int main(){
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to store request packet for sending
	Buffer requestPacket;

	//Buffer to store the return from RDLPIM
	Buffer responce;

	//Helper class to build the header packet
	RequestHeader subscribeReq;

	//a DataElement to store the data returned from the RDLPIM
	DataElement reqVariable("th_temp_eff");

	//Array of DataElements to be recieved from the RDLPIM
	DataElementArray dataRecieved;

	//build the header packet
	//set the command
	subscribeReq.SetCommand(Commands::subscribe);
	//set the data size
	requestPacket = subscribeReq.SetData(reqVariable.SerialiseName());


	//Send the request
	if (client.CanSend())
	{
		client.Send(requestPacket);
	}

	//Recieve the updates and print them
	while (true) {
		if (client.CanRead()) {
			client.Recieve(responce);

			//Deserialse the header and strip it from the responce so you are left with the raw data
			subscribeReq.ProcessHeader(responce);

			//if recieved data from RDLPIM, deserialise it
			if (subscribeReq.GetCommand() == Commands::data) {
				//deserialise data to a DataElement so its size and data are easily accessable.
				dataRecieved.Deserialise(responce);
			}

			//print out variables recieved and their types
			for(auto element : dataRecieved){
				std::cout << element.GetName() << "is type " << element.GetType() << std::endl;
			}
		}
	}
}

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class SubscribeRequest{

	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController;

	//Container to store recieved RDLPIM data within
	public List<DataElement> dataRecieved;


	//Constructor to initialise the reference to the RDLPIM interface
	// and subscribe to the event raised upon recieving new data. This event passes the data recieved in the form
	// of a list of DataElements
	SubscribeRequest()
	{
		rdlController = RDLPIM_Controller.Instance;
		rdlController.DataRecieved += onNewData;
	}

	//It is important to unsubscribe call back on the event the object get destroyed (scene change or object deleted from scene) failure to do so will 
	//raise errors at runtime and reduce frame rate
	public void OnDestroy
	{
		rdlController.DataRecieved -= onNewData;
	}


	//Method to send multiple Subscribe requests,
	//variables names are passed as a list of strings
	public void SendSubscribes(List<string> vars)
	{
		List<DataElement> reqs = new List<DataElement>();
		foreach(string varname in vars){
			reqs.Add(new DataElement(varname));
		}

		rdlController.SendSubscribe(reqs);
	}


	//Callback for the onDataRecieved event on the RDLPIM interface class (RDLPIM_Controller)
	// This method simply copies the received data into this instance of the SubscribeRequest class
	protected void onNewData(object source, List<DataElement> newData)
	{
		dataRecieved = newData;
	}
}
~~~~~~~~~~~~~~~~~~~~~~~









\subsubsection apiUnsubscribe API Command - Unsubscribe Data
The Unsubscribe data request code is used to unsubscribe from any \ref apiSubscribe "subscriptions" made to the RDLPIM . Upon recieving a Unsubscribe data request, the RDLPIM will terminate any \ref apiData "Data" packets being sent to a client following the event of data change.

\warning There is no responce from the RDLPIM to aknowledge the unsubscription request, the client will simply no longer recieve packets from the subscription.  This is a concious design decision to improve the realtime performance of the RDLPIM, should you require that the RDLPIM
provides positive feedback to an Unsubscribe request, please let the lead developer know and or raise an issue on the <a href="https://github.com/VRDL-Covid/RDLPIM/issues">RDLPIM's GitHub issues page.</a>

The first 4 bytes of a Subscribe request is the value of the unsubscribe request code, as of RDLPIM V1.0 is (Int32)6 (0x06 0x00 0x00 0x00). The next 4 bytes is the int32 representation of the length in bytes of the request data. For an unsubscribe request packet 
the request data contains the variable names you wish to unsubscribe from the RDLPIM, multiple variables can be stacked by encapsulating them with curly braces, curly braces are therefore illegal characters for a variable name


 The Table below shows a simple subscribe request architecture in raw bytes.

####Example Unsubscribe Request Packet
 * byte | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11 | 12| 13| 14| 15| 16| 17| 18| 19| 20
 * -----|:----:|----|----|----|:----:|----|----|----|:---:|---|---|---|---|---|---|---|---|---|---|---|---
 * **Description**|Function Code||||Size of Request Data||||Request Data|||||||||||||
 * **Hex/Char**|0x06|0x00|0x00|0x00|0x0D|0x00|0x00|0x00|'{'|'t'|'h'|'_'|'t'|'e'|'m'|'p'|'_'|'e'|'f'|'f'|'}'
 * **Human Readable**|6||||13||||{th_temp_eff}|||||||||||||
 * **Length(bytes)**|4||||4||||13|||||||||||||


\warning **1)** Function code/command code values are likely to change through time as the RDLPIM is extended. It is therefore recommended that the enumerator class \ref Commands is exported to generate concurant request codes ed Commands::unsubscribe.


\warning **2)** '{' and '}' are illegal characters for a variable name



Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to build an usubscribe request 

#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h


int main(){
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to store request packet for sending
	Buffer requestPacket;

	//Helper class to build the header packet
	RequestHeader unsubscribeReq;

	//a DataElement to store the data returned from the RDLPIM
	DataElement reqVariable("th_temp_eff");

	//build the header packet
	//set the command
	unsubscribeReq.SetCommand(Commands::unsubscribe);
	//set the data size
	requestPacket = unsubscribeReq.SetData(reqVariable.SerialiseName());


	//Send the request
	if (client.CanSend())
	{
		client.Send(requestPacket);
	}
}

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class UnsubscribeRequest{

	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController;

	//Container to store recieved RDLPIM data within
	public List<DataElement> dataRecieved;


	//Constructor to initialise the reference to the RDLPIM interface
	UnsubscribeRequest()
	{
		rdlController = RDLPIM_Controller.Instance;
	}


	//Method to send multiple Unsubscribe requests,
	//variables names are passed as a list of strings
	public void SendUnsubscribes(List<string> vars)
	{
		List<DataElement> reqs = new List<DataElement>();
		foreach (string varname in vars)
		{
			reqs.Add(new DataElement(varname));
		}

		rdlController.SendUnsubscribe(reqs);
	}
}
~~~~~~~~~~~~~~~~~~~~~~~


\subsection apiChat API Command - ASCII Chat

The RDLPIM is currently able to broadcast messages from one connected client to the remainder of the connected clients.  This behaviour is achieved by sending a Chat request packet.  As with all client side request functions, the first 4 bytes of the request is the function
code value for a chat request (int32)2 (0x02 0x00 0x00 0x00). The next 4 bytes is the length, in bytes, of the request data. The request data forms the character array which is the message to be sent.

 The Table below shows the architecture of a simple chat request packet in raw bytes.
####Example Chat Request Packet
 * byte | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11 | 12| 13| 14| 15| 16| 17| 18| 19
 * -----|:----:|----|----|----|:----:|----|----|----|:---:|---|---|---|---|---|---|---|---|---|---|---
 * **Description**|Function Code||||Size of Request Data||||Request Data||||||||||||
 * **Hex/Char**|0x02|0x00|0x00|0x00|0x0C|0x00|0x00|0x00|'H'|'e'|'l'|'l'|'o'|''|'W'|'o'|'r'|'l'|'d'|'!'
 * **Human Readable**|2||||12||||Hello World!||||||||||||
 * **Length(bytes)**|4||||4||||12||||||||||||
 
The message recieved by a client has the same form as the request packet, so any chat messages received by a client from the RDLPIM will have the same header structure.

Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to **Send** and **Recieve** Chat messages

#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h

int main(){
	//Connect to the RDLPIM
	rdlpimClient client("127.0.0.1", 8000);
	client.connectToRDLPIM();

	//Buffer to store message for sending and recieve messages to
	Buffer message;
	message.set("Hello World!");

	//Helper class to build the header packet
	RequestHeader chatReq;

	//build the header packet
	//set the command
	chatReq.SetCommand(Commands::chat);
	//set the data size
	message = chatReq.SetData(message);


	//Broadcast a single Chat message
	if (client.CanSend()) {
		client.Send(message);
	}

	// listen for incoming chat for ever!
	while (true) {
		//receive incoming from RDLPIM
		if (client.CanRead())
		{
			client.Recieve(message);

			//Process the header so the type of recieved can be deduced
			chatReq.ProcessHeader(message);

			//if recieved chat, print the message to the console
			if (chatReq.GetCommand() == Commands::chat) {
				std::cout << message << std::endl;
			}
		}
	}
}

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class Chat{

	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController;

	//Constructor to initialise the reference to the RDLPIM interface and subscribe to the incoming chat data event provided
	Chat()
	{
		rdlController = RDLPIM_Controller.Instance;
		rdlController.ChatRecieved += onChatRecieved;
	}
	
	//unsubscribe on destroyed/finalised
	~Chat()
	{
		rdlController.ChatRecieved -= onChatRecieved;
	}

	//When chat recieved, write it to the console, callback to rdlController.ChatRecieved provided in example project include folder
	protected void onChatRecieved(object source, string message)
	{
		Console.Write(message)
	}

	//Method to broadcast chat to clients connected to the RDLPIM
	public void BroadcastChat(string message)
	{
		rdlController.SendChat(message);
	}

}
~~~~~~~~~~~~~~~~~~~~~~~



\subsection apiVOIP API Command - VOIP Data

\warning This is a place holder for RDLPIM VOIP integration (see Matthew Stanwell for update) **DO NOT USE**



\subsection apiRecieved Client Side Recieved Commands

\subsubsection apiData API Command - Recieved Data Packet
When data is served to a client (as a responce to a \ref apiPull "Pull" or \ref apiSubscribe "Subscribe") it is done so in the form of a **Data Packet**.  The first 4 byts of the packet will be the \ref Commands "function code / Command Code" for a data packet.  
As of RDLPIM v1.0 this is (int32)1 (0x01 0x00 0x00 0x00).  The next 4 bytes represents the length, in bytes (int32 format), of the total data served to the client, this could be multiple variables stored within the RDLPIM.  
The data served is in the form of a chain of serialised DataElemets, each data element is containerised by open and closed curly brackets '{ }'. A DataElement contains two key words, the variable name and its type, these key words are delimited by an '=' character.
After the second '=' character, the next 4 bytes is the length in bytes (int32 format) of the raw data contained within the DataElement, the remainder of the DataElement is the raw data in binary format.  A DataElement can therefore represent any digital data format, a hint to its
type should be included within the 'Type' partition of the DataElement.

Lets consider a simple example of a DataElement containing a variable called 'pi', of type 'Double', and value 3.14159. This DataElement wouldserialise to:

####Serialised DataElement
 * byte        | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11  |  12|  13|  14|  15|  16|  17|  18|  19|  20|  21|  22|  23
 * -----       |:----:|:----:|----|:----:|:----:|----|----|----|---|---|:---:|:----:|----|----|----|:----:|----|----|----|----|----|----|----|:----:
 * **Description**|Start DataElement Delimiter|VarName||Varname Delimiter|Type||||||Type Delimiter|DataSize||||Data in Binary||||||||End DataElement Delimiter|
 * **Hex/Char**|'{' |'p' |'i' |'=' |'d' |'o' |'u' |'b' |'l'|'e'|'='|0x08|0x00|0x00|0x00|0x40|0x09|0x21|0xF9|0xF0|0x1B|0x86|0x6E|'}'
 * **Human Readable**|{|pi||=|double||||||=|8||||(double)3.14159||||||||}|
 
 If the RDLPIM were to serve this data alone to a client, below represents the full data packet recived. Please note however that multiple DataElements can be stacked within this packet.

 ####Example Recieved Data Packet
 * byte       | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8 | 9 |10 |11  |  12|  13|  14|  15|  16|  17|  18|  19|  20|  21|  22|  23|  24|  25|  26|  27|  28|  29|  30|  31
 *        -----|:----:|:----:|:----:|:----:|:----:|----|:----:|:----:|:---:|---|:---:|:----:|----|----|----|:----:|----|----|----|----|----|----|----|:----:|----|----|----|----|----|----|----|----
 * **Description**|Function Code||||Size of Request Data||||Request Data||||||||||||||||||||||||
 * **Hex/Char**|0x01|0x00|0x00|0x00|0x18|0x00|0x00|0x00|'{' |'p' |'i' |'=' |'d' |'o' |'u' |'b' |'l'|'e'|'='|0x08|0x00|0x00|0x00|0x40|0x09|0x21|0xF9|0xF0|0x1B|0x86|0x6E|'}'
 * **Human Readable**|1||||24||||DataElement Serialised||||||||||||||||||||||||
 * **Length bytes**|4||||4||||24||||||||||||||||||||||||

Below are examples of how to use the helper classes available within the gwcRDLToolbox and example unity project to Recieve data packets from the RDLPIM and deserialise them
#### Example with helper classes from gwcRDLToolbox (C++)

~~~~~~~~~~~~~~~~~.cpp
#include gwcRDLToolBox.h

int main(){
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

~~~~~~~~~~~~~~~~~

#### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class Data
{
	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController;

	//Constructor to initialise the reference to the RDLPIM interface and subscribe to the incoming data event provided
	Data()
	{
		rdlController = RDLPIM_Controller.Instance;
		rdlController.DataRecieved += onDataRecieved;
	}

	//unsubscribe on destroyed/finalised
	~Data()
	{
		rdlController.DataRecieved -= onDataRecieved;
	}

	//When data recieved, write its name and type to the console, callback to rdlController.onDataRecieved provided in example project include folder
	protected void onDataRecieved(object source, List<DataElement> data)
	{
		foreach (DataElement element in data)
		{
			Console.Write(element.GetName() + "of type:" + data.GetType());
		}
	}
}
~~~~~~~~~~~~~~~~~~~~~~~




\subsubsection apiInfo API Command - RDLPIM Server Info
The RDLPIM will serve clients information about the status of the RDLPIM and its clients, these messages will be served to the client as an Info packet. Server info packets
can be handelled identically to \ref apiChat "Chat" messages.  The only notable difference is that the \ref Commands "function / command code" is (int32)8 (0x08 0x00 0x00 0x00). The data contained within 
Info packets is a human readable string containing server/client status information

\subsubsection apiErrr API Command - Error Packet
The RDLPIM will serve clients error information should a fault occur, for example incorrectly formatted request packet, these messages will be served to the client as an Error packet. Server Error packets
can be handelled identically to \ref apiChat "Chat" messages.  The only notable difference is that the \ref Commands "function / command code" is (int32)9 (0x09 0x00 0x00 0x00). The data contained within 
Error packets is a human readable string containing information regarding the fault condition.
*/

