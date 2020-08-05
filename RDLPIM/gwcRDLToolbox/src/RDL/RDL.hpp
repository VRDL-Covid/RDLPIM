#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include"DataBase/DataBase.h"
#include"Core/buffer.hpp"
#include"s3Includes.hpp"
#include"rdlData.hpp"
#include"Core/Event.h"



//Define boolean shortcut
#define on 0xFF
#define off 0x00


// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle interaction with the RDL, constructed by passing a buffer* / char* containing the processes name eg "rtex10"
class RDL
{
public:
	DWORD pid;

	~RDL();

	static RDL* Get();

	void worker(bool& work);


	void Init(const char* processName);

	// plcGetVarAddress:
	// input as character array
	// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
	long FindRuntimePointer(char* varname);

	//RDL_Active deteremins if the RDL is still running.
	bool RDL_Active();


	rdlData Read(const char* varname);
	
	rdlData Read(char* varname);

	//templateised write memory functions
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


	void Write(const char* varName, char* data, size_t size);
	void Write(const std::string& varName, char* data, size_t size);
	void Write(const DataElement& data);

	void TrackVariable(const rdlData& varName);
	void UntrackVariable(const std::string& varName);

public://callbacks
	Ref<EventCallback<const std::string&>> c_NewDataEntry;
	Ref<EventCallback<const DataElement&>> c_DB_ElementChanged;
private:
	static RDL* s_Instance;

	std::unordered_map<std::string,Buffer> m_trackedVars;
	std::mutex m_TrackedVarsArray;

private:
	RDL();
	bool OnNewVariableHandler(const std::string& varName);
	bool OnDataElementChanged(const DataElement& data);
};