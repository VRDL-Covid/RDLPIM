#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include"DataBase/DataBase.h"
#include"Core/buffer.hpp"
#include"s3Includes.hpp"
#include"rdlData.hpp"
#include"Core/Event.h"



/// @brief Fortran 70 compliant True
/// 
/// If setting a boolean (L1) within the RDL to be true, use this macro otherwise RDL behaviour is undefined. (and honestly really quite odd)
#define on 0xFF

/// @brief Fortran 70 compliant False
#define off 0x00

/// @brief A class to handle the interface between external code and the RDL's runtime memory and execution
/// 
/// This class follows the singleton design pattern as there can only be one instance of an RDL per machiene.  An instantiated RDL object needs to be initialised with the Init(const char* processName) method. The RDL 
/// Object maintains a list of variables within the DataBase instanec which it owns, it periodically updates these values within the DataBase envoking onChanged events which ultimately propogate the sending of subscription
/// driven data packets to connected and subscribed clients. This behaviour is handled by the worker() method which should be placed on its own thread.
/// 
/// ###Example
/// ~~~~~~~~~~~~.cpp
/// 
/// // ********************** //
/// //		Get and init      //
/// // ********************** //
/// 
/// //get an instance of the RDL singleton
/// RDL* theRDL = RDL::Get();
/// 
/// //initialise it
/// theRDL->Init("rtex10.exe")
/// 
/// 
/// // ********************** //
/// //	Start Worker Thread   //
/// // ********************** //
/// bool work = true;
/// std::thread rdlManager(&RDL::worker, theRDL, std::ref(work));
/// 
/// 
/// // ********************** //
/// //		Write Data        //
/// // ********************** //
/// 
/// //variable and value to write to
/// std::string varname("th_temp_eff");
/// double value = 40.0;
/// 
/// //Write to the RDL's runtime memory
/// theRDL->Write(varname, value);
/// 
/// 
/// // ********************** //
/// //	    Read Data         //
/// // ********************** //
/// 
/// Buffer readDataRaw;
/// double readDouble;
/// 
/// rdlData data = theRDL->Read("th_temp_eff");
/// 
/// readDataRaw = data.GetData();
/// readDouble = *(double*)readDataRaw.GetContents();
/// 
/// // ********************** //
/// //	    Exit			  //
/// // ********************** //
/// 
/// work = false;
/// rdlManager.join();
/// ~~~~~~~~~~~~
/// 
/// @warning The RDL object must be initialised with Init(const char* processName)
class RDL
{
public:
	DWORD pid;///< process ID of the RDL's process

	~RDL();

	/// @brief A static method to instantiate if nessesary and return a raw pointer to an RDL object
	/// @return A raw pointer to an RDL object
	static RDL* Get();

	/// @brief Background worker function to be placed on its own thread
	/// 
	/// This function keeps data within a DataBase instance up to date if it is 'owned' by the RDL. This thread is joinable once the referenc argument bool& work is false
	/// @param work status of execution.
	void worker(bool& work);

	/// @brief This method is used to find the process ID of the RDL, required to gain access to the RDL's process memory
	/// 
	/// The name of the process must be spcified here, it will likely be "rtex10.exe" but can be another Simexec executable.
	/// @param processName Process name, eg "rtex10.exe"
	void Init(const char* processName);

	
	/// @brief Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
	/// @param varname variable name eg "th_temp_eff"
	/// @return long ptr
	long FindRuntimePointer(char* varname);

	/// @brief Return the status of the RDL's process id, if the RDL has launched this will return true
	/// @return RDL status
	bool RDL_Active();

	/// @brief A method to get an rdlData runtime object.
	/// 
	/// This will return an rdlData object containing the data type, raw data, memory address.  For a complete set of contents see rdlData documentation.
	/// @param varname variable name
	/// @return rdlData opject (copy)
	rdlData Read(const char* varname);
	
	/// @brief A method to get an rdlData runtime object.
	/// 
	/// This will return an rdlData object containing the data type, raw data, memory address.  For a complete set of contents see rdlData documentation.
	/// @param varname variable name
	/// @return rdlData opject (copy)
	rdlData Read(char* varname);


	/// @brief A method to write a primitive value into the RDL's runtime memory, for example an bool int float or double
	/// @tparam T Data type
	/// @param varName name of variable within the RDL to write to
	/// @param value value data
	template<typename T>
	void Write(const std::string& varName, T value)
	{
		char data[sizeof(T)];
		memcpy(&data[0], &value, sizeof(T));

		char* _name;
		strcpy(_name, varName.c_str());

		int iResult;

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

		iResult = WriteProcessMemory(processHandle, (LPVOID)FindRuntimePointer(_name), &data[0], sizeof(T), NULL);
	}

	/// @brief specialised RDL write template accounting for Fortan70 boolean true spec
	/// @param varName name of variable within the RDL to write to
	/// @param value value data
	template<>
	void Write<bool>(const std::string& varName, bool value)
	{
		char data[sizeof(bool)];
		memcpy(&data[0], &value, sizeof(bool));

		char _name[S3_STRLEN];
		strcpy_s(_name, (rsize_t)S3_STRLEN, varName.c_str());

		int iResult;

		if (value) {
			data[0] = (char)0xff;
		}
		else {
			data[0] = (char)0x00;
		}

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

		iResult = WriteProcessMemory(processHandle, (LPVOID)FindRuntimePointer(_name), &data[0], sizeof(bool), NULL);
	}

	/// @brief A method to write complex data structures to the RDL runtime memory
	/// @param varName name of variable within the RDL to write to
	/// @param data a pointer to the data
	/// @param size amount of data in bytes to write
	void Write(const char* varName, char* data, size_t size);

	/// @brief std::string varname overload of Write(const char* varName, char* data, size_t size);
	/// @param varName name of variable within the RDL to write to
	/// @param data a pointer to the data
	/// @param size amount of data in bytes to write
	void Write(const std::string& varName, char* data, size_t size);

	/// @brief A method to write the contents of a DataElement object to the RDL's runtime memory
	/// @param data source data
	/// @warning the variable name within the DataElement object must be aligned with the RDL's DBM
	void Write(const DataElement& data);

	/// @brief A method to add a variable to the by tracked for changes within the RDL's runtime.  Should a change be detected, the DataBase instance's copy will be made consistent.
	/// @param varName rdlData refernce object
	void TrackVariable(const rdlData& varName);

	/// @brief A method to stop tracking a specific variable within the RDL, the DataBase's copy will no longer be kept live.
	/// @param varName name of variable within the RDL to stop tracking
	void UntrackVariable(const std::string& varName);

public://callbacks
	Ref<EventCallback<const std::string&>> c_NewDataEntry;  ///< on new entry to DataBase callback
	Ref<EventCallback<const DataElement&>> c_DB_ElementChanged; ///< on RDL owned data change within DataBase call back, handel pushed data from a connected client
private:
	static RDL* s_Instance;

	std::unordered_map<std::string,Buffer> m_trackedVars;
	std::mutex m_TrackedVarsArray;

private:
	RDL();
	bool OnNewVariableHandler(const std::string& varName);
	bool OnDataElementChanged(const DataElement& data);
};