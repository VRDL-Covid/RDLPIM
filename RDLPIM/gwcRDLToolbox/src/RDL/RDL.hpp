#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include"Core/buffer.hpp"
#include"s3Includes.hpp"



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

	void Init(const char* processName);

	// plcGetVarAddress:
	// input as character array
	// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
	long plcGetVarAddress(char* varname);

	//RDL_Active deteremins if the RDL is still running.
	bool RDL_Active();


	//templatised read memory functions.
	template<typename T>
	T Read(INT_PTR address)
	{
		char* buffer[sizeof(T)];
		SIZE_T bytesRead;
		T retVar;


		HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, pid);

		if (ReadProcessMemory(processHandle, (LPCVOID)address, buffer, sizeof(T), &bytesRead)) {
			CloseHandle(processHandle);
			retVar = *(T*)buffer;
		}
		else {
			printf("WARNING::Error reading process memory at Address:0x%p\n", address);
			return false;
		}

		return retVar;
	}

	template<typename T>
	T Read(const char* varname)
	{
		char temp[64];
		strcpy_s(temp, varname);
		return Read<T>(plcGetVarAddress(temp));
	}

	template<typename T>
	T Read(char* varname)
	{
		return Read<T>(plcGetVarAddress(varname));
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
	
	template<>
	void Write<bool>(INT_PTR address, bool value)
	{
		char data[sizeof(bool)];
		memcpy(&data[0], &value, sizeof(bool));

		int iResult;

		if (value) {
			data[0] = 0xff;
		}
		else {
			data[0] = 0x00;
		}

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

		iResult = WriteProcessMemory(processHandle, (LPVOID)address, &data[0], sizeof(bool), NULL);
	}

private:
	static RDL* s_Instance;

private:
	RDL();
};