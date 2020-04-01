#include "RDL.hpp"
#include<stdio.h>
#include<iostream>

// Import DLLs required to read RDL global memory.
// this is required in order to determine the memory allocation during
// run time of the RDL.
extern "C" {
	// imported functions from s3dll.dll
	typedef void(__stdcall  *__ET)();
	typedef struct {
		__ET entry;
		char	*longname;
		char	*user;
		char	type, cons, dummy, level;
		int	argc;
		unsigned int mflags;
	} MODTAB;

	__declspec(dllimport) int mdd_open(char* who);
	__declspec(dllimport) void mdd_close(int which);
	__declspec(dllimport) int DbmPidNew(int s, char *instring, int lreq, char *typedb,
		char *global, int *offset, char *type1, int *prec, int *dimen, char *vcopt,
		long *ival, double *dval, int *lfound, char *sdesc, char *ldesc, char *unit,
		char *sys_id, char *fmt, unsigned int *date);

	// imported data from mstg.dll
	__declspec(dllimport) int number_shared_globals;
	__declspec(dllimport) GLOBAL_TABLE share_global_table[];

};

//Guy Collins 17/11/2016
// Obtain Process ID of a generic name eg "rtex10.exe"
DWORD GetPID(const std::string& processName)
{

	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile)) {
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo)) {
		if (!processName.compare(processInfo.szExeFile)) {
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}



RDL::RDL()
{
}

RDL::RDL(const buffer &ipid)
{
	pid = GetPID(ipid.contents);
}

RDL::RDL(const char* ipid)
{
	pid = GetPID(ipid);
}

RDL::RDL(char* ipid)
{
	pid = GetPID(ipid);
}

RDL::~RDL()
{
}

// readMemDouble
// Read double variable from memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
double RDL::readMemDouble(INT_PTR address)
{
	const int bufferSize = 8;
	char* buffer[bufferSize];
	SIZE_T bytesRead;
	double retVar;


	HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, pid);

	if (ReadProcessMemory(processHandle, (LPCVOID)address, buffer, bufferSize, &bytesRead)) {
		CloseHandle(processHandle);
		retVar = *(double*)buffer;
	}
	else {
		printf("WARNING::Error reading process memory at Address:0x%p\n", address);
		return NULL;
	}

	return retVar;
}


// readMemInt
// Read integer variable from memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
int RDL::readMemInt(INT_PTR address)
{
	const int bufferSize = 4;
	char* buffer[bufferSize];
	SIZE_T bytesRead;
	int retVar;


	HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, pid);

	if (ReadProcessMemory(processHandle, (LPCVOID)address, buffer, bufferSize, &bytesRead)) {
		CloseHandle(processHandle);
		retVar = *(int*)buffer;
	}
	else {
		printf("WARNING::Error reading process memory at Address:0x%p\n", address);
		return -1;
	}

	return retVar;
}

// readMemBool
// Read Boolean variable from memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
bool RDL::readMemBool(INT_PTR address)
{
	const int bufferSize = 1;
	char* buffer[bufferSize];
	SIZE_T bytesRead;
	bool retVar;


	HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, pid);

	if (ReadProcessMemory(processHandle, (LPCVOID)address, buffer, bufferSize, &bytesRead)) {
		CloseHandle(processHandle);
		retVar = *(bool*)buffer;
	}
	else {
		printf("WARNING::Error reading process memory at Address:0x%p\n", address);
		return false;
	}

	return retVar;
}


// plcGetVarAddress:
// input as character array
// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
long RDL::plcGetVarAddress(char* varname)
{
	char name[S3_STRLEN];
	char global[S3_STRLEN];
	int offset;
	char ctype;
	int iprec = 0;
	int level = 1;         //send 1 for ods only lookup
	int dimen[7] = { 0,0,0,0,0,0,0 };
	int lreq = 2;        // #define UO_LEV	2
	int iResult;

	sprintf_s(name, S3_STRLEN, "%s", varname);

	// Find Global memory block (eg "global_v") in which varname (eg "th_temp_eff") is stored
	// Find the offset from the runtime base memory location of the global memory block
	iResult = mdd_open(0);
	if (iResult == 0) printf("MST_XFER : mdd_open=%d\n", iResult);
	iResult = DbmPidNew(0, name, lreq, 0, global, &offset, &ctype, &iprec, dimen, 0, 0, 0, &level, 0, 0, 0, 0, 0, 0);

	if (iResult == DBM_PID_ERROR) {
		printf("WARNING::Variable %s not in dbm.\n", name);
		return 0;
	}

	// Pointer arithmetic to return the runtime location within the memory (RAM) that the input
	// variable is stored at.
	for (int i = 0; i < number_shared_globals; i++) {
		if (_stricmp(global, share_global_table[i].name) == 0) {
			long result = (long)share_global_table[i].base + offset;
			return result;
		}
	}
	mdd_close(3);
	return 0;
}





bool RDL::RDL_Active()
{
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}

// WriteMem(Bool overload)
// Write 1 byte bool to memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
void RDL::WriteMem(INT_PTR address, bool value)
{

	convertable buffer;
	buffer.Double = 0;
	buffer.Bool = value;
	int iResult;

	if (value) {
		buffer.Char[0] = 0xff;
	}
	else {
		buffer.Char[0] = 0x00;
	}

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	iResult = WriteProcessMemory(processHandle, (LPVOID)address, &buffer, sizeof(bool), NULL);
}

// WriteMem(Int overload)
// Write 4 byte int to memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
void RDL::WriteMem(INT_PTR address, int value)
{

	convertable buffer;
	buffer.Double = 0;
	buffer.Int = value;
	int iResult;

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	iResult = WriteProcessMemory(processHandle, (LPVOID)address, &buffer, sizeof(int), NULL);
}


// WriteMemDouble(double overload)
// Write 8 byte double to memory space
// specify the hex address within the memory that you would like to read
// and the PID that the memory has been allocated to. Required under Windows OS.
void RDL::WriteMem(INT_PTR address, double value)
{

	convertable buffer;
	buffer.Double = 0;
	buffer.Double = value;
	int iResult;

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	iResult = WriteProcessMemory(processHandle, (LPVOID)address, &buffer, sizeof(double), NULL);
}


// readDouble
// Read double variable from RDL
double RDL::readDouble(char* varname)
{
	return readMemDouble(plcGetVarAddress(varname));
}

// readDouble
// Read double variable from RDL
double RDL::readDouble(const char* varname)
{
	char temp[64];
	strcpy_s(temp, varname);
	return readMemDouble(plcGetVarAddress(temp));
}


// readMemInt
// Read integer from RDL
int RDL::readInt(char* varname)
{
	return readMemInt(plcGetVarAddress(varname));
}

// readMemInt
// Read integer from RDL
int RDL::readInt(const char* varname)
{
	char temp[64];
	strcpy_s(temp, varname);
	return readMemInt(plcGetVarAddress(temp));
}


// readMemBool
// Read Boolean from RDL
bool RDL::readBool(char* varname)
{
	return readMemBool(plcGetVarAddress(varname));
}

// readMemBool
// Read Boolean from RDL
bool RDL::readBool(const char* varname)
{
	char temp[64];
	strcpy_s(temp, varname);
	return readMemBool(plcGetVarAddress(temp));
}


