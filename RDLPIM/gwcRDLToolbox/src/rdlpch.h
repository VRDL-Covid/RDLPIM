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
 * Phone:07534262038
 * 
 * \section install_sec Compilation and Installation
 *
 * 
 * 
 * 
 * 
 * 
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
 * \subsection compiling Step3 : Compiling
 * 
 * The visual studio has several solution configurations to compile the RDLPIM for either the end user or developer.  The table below outlines the available configurations and who they are intended for.
 * 
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
 * \subsection running Step3 : Execution
 * 
 * The RDLPIM requires access to DLLs provided by the GSE suite inorder to determine the runtime location of variables within the RDL.  In order for the RDLPIM to be executed, a GSE "S:" drive will need to be
 * mapped and the RDLPIM binary launched via a CMDLOAD console.  This is required to ensure the correct environmental parameters are set and DLL access is available. 
 * 
 * At this stage you will be greeted with a welcome message deatiling the version of the RDLPIM being executed and its status.
 * 
 * \warning
 * For convenience and home testing during the Covid crisis, the required DLLs will be copied next to the binary upon building, for onsite us, delete these DLLs to enure the correct ones are referenced via the S: drive.
 */

 /*! \page APIUG API User's Guide

 \par Table of Contents

 - \ref users_guide
    - \ref apiIntro "API Introductin"
    - \ref apiConnection "API Connection and Handshake"
    - \ref Unity Implimentation


\subsection apiIntro "API Introduction"
    hello there!
 */