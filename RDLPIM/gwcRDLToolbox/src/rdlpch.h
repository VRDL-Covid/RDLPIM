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
    - \ref apiIntro "API Introductin"
    - \ref apiConnection "API Connection and Handshake"
        - \ref apiPull "Pull Data"
        - \ref apiPush "Push Data"


\subsection apiIntro API Introduction
    Connections and requests made to the RDLPIM are done so by a custom API.  This section of the documentation provides a detailed description of how to build connection and request packets, as well as
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

###Example

~~~~~~~~~~~~.cpp
    rdlpimClient client1("127.0.0.1", 8000);
    client1.connectToRDLPIM();
    Buffer recieved;


    bool SomethingToSend;
    while (true) {

		if (client1.CanRead()) {
            client1.Recieve(&recieved);
        }

        if(SomethingToSend && client1.CanSend(){
            client1.Send("some message");
        }
    }
~~~~~~~~~~~~



\subsection APICommands API Commands/Functions

A client is able to Push, Pull, Subscribe and Unsubscribe from data stored within the RDLPIM/RDL,  The client does not need to make a destinction between whether the data requested exists on the RDLPIM or RDL runtime, this is handled by the RDLPIM. 
In addition to the data manipulating requests, function codes denoting a data packet, server info and inter-client chat also exist.  
This section of the documentation attempts to explain in detail how to build these packets from the ground up, and whathelper classes exist to help you serialise and deserialise messages to and from the RDLPIM.

All requests made to the RDLPIM are done so via an Asynchronous TCP socket, negotiatied following \ref apiConnection "connection with the RDLPIM".  
Request packets start with an 8byte header, the first 4 bytes are the int32 represnetation of the \ref Commands "function code" for example pull = 4 = 0x04 0x00 0x00 0x00.  
The next 4 bytes is the int32 representation of the length in bytes of the remaining data within the request packet. Please see below for detailed examples of how to build request packets and how to deserialise their result.

\subsection apiPull API Command - Pull Data

The Pull data request code is used to Pull data from the RDLPIM, the value of the pull request code as of RDLPIM V1.0 is (Int32)4 (0x04 0x00 0x00 0x00) and forms the first 4 bytes of the request header packer. The next 4 bytes is the int32 representation of the length in bytes of the request data.
The request data contains the variable names you wish to pull from the RDLPIM, multiple variables can be stacked by encapsulating them with curly braces, curly braces are therefore illegal characters 
for a variable name

The Table below shows a simple pull request architecture for  in raw bytes.

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

### Example with helper classes from gwcRDLToolbox (C++)

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
			client.Recieve(&responce);
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

### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class PullRequest{

	//get instance to the provided RDLPIM interface singleton
    private RDLPIM_Controller rdlController

	//Container to store recieved RDLPIM data within
	public List<DataElemet> dataRecieved;


	//Constructor to initialise the reference to the RDLPIM interface
	// and subscribe to the event raised upon recieving new data. This event passes the data recieved in the form
	// of a list of DataElements
    PullRequest()
    {
        rdlController = RDLPIM_Controller.Instance;
        rdlController.DataRecieved += onNewData;
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





\subsection apiPush API Command - Push Data
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

### Example with helper classes from gwcRDLToolbox (C++)

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

### Example with helper classes from Providied Example Unity Project (C#)
see <a href="unityClient.zip">Example Unity Project.</a>

~~~~~~~~~~~~~~~~~~~~~~~.cs

class PushRequest{

	//get instance to the provided RDLPIM interface singleton
	private RDLPIM_Controller rdlController

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
*/
