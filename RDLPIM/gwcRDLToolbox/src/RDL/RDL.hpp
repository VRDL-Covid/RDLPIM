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


// type to convert types prior to sending over TCP/IP using char array buffer.
union convertable {
	char Char[8];
	double Double;
	float Float;
	int Int;
	bool Bool;
};




// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle interaction with the RDL, constructed by passing a buffer* / char* containing the processes name eg "rtex10"
class RDL
{
public:
	DWORD pid;

	~RDL();

	static RDL* Get();

	void worker(bool& work)
	{
		while (work) {

		}
	}

	void Init(const char* processName);

	// plcGetVarAddress:
	// input as character array
	// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
	long plcGetVarAddress(char* varname);

	//RDL_Active deteremins if the RDL is still running.
	bool RDL_Active();


	rdlData Read(const char* varname)
	{
		return rdlData(varname);
	}
	
	rdlData Read(char* varname)
	{
		return rdlData(varname);
	}

	//templateised write memory functions
	template<typename T>
	void Write(INT_PTR address, T value)
	{
		char data[sizeof(T)];
		memcpy(&data[0], &value, sizeof(T));

		int iResult;

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

		iResult = WriteProcessMemory(processHandle, (LPVOID)address, &data[0], sizeof(T), NULL);
	}

	void Write(const char* varName, char* data, size_t size)
	{
		char* _name ;

		strcpy(_name, varName);
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		WriteProcessMemory(processHandle, (LPVOID)plcGetVarAddress(_name),data, size, NULL);
	}

	template<>
	void Write<bool>(INT_PTR address, bool value)
	{
		char data[sizeof(bool)];
		memcpy(&data[0], &value, sizeof(bool));

		int iResult;

		if (value) {
			data[0] = (char)0xff;
		}
		else {
			data[0] = (char)0x00;
		}

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

		iResult = WriteProcessMemory(processHandle, (LPVOID)address, &data[0], sizeof(bool), NULL);
	}


	void TrackVariable(const std::string& varName);
	void UntrackVariable(const std::string& varName);

public://callbacks
	Ref<EventCallback<const std::string&>> c_NewDataEntry;
	Ref<EventCallback<const DataElement&>> c_DB_ElementChanged;
private:
	static RDL* s_Instance;

	std::vector<std::string> m_trackedVars;
	std::mutex m_TrackedVarsArray;

private:
	RDL();
	bool OnNewVariableHandler(const std::string& varName);
	bool OnDataElementChanged(const DataElement& data);
};