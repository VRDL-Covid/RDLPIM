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
 * \section install_sec Compilation and Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 * 
 */

 /*! @page APIUG API User's Guide

 @par Table of Contents

 - @ref users_guide
    - @ref Introduction
    - @ref api_overview
    - @ref Unity Implimentation

 */