#include "rdlpch.h"
#include "rdlData.hpp"
// Import DLLs required to read RDL global memory.
// this is required in order to determine the memory allocation during
// run time of the RDL.

DWORD rdlData::pid = -1;

std::mutex rdlData::s_readProcessLock;

void rdlData::initRDLData(const char* varname)
{
	int iResult;
	int offset;


	sprintf_s(name, S3_STRLEN, "%s", varname);

	iResult = mdd_open(0);
	if (iResult == 0) printf("MST_XFER : mdd_open=%d\n", iResult);

	iResult = DbmPidNew(s,
		name,
		lreq,
		typeDB,
		global,
		&offset,
		ctype,
		&bytes,
		dimen,
		vcopt,
		&ival,
		&dval,
		&level,
		sdesc,
		ldesc,
		unit,
		sys_id,
		fmt,
		&date);

	if (iResult == DBM_PID_ERROR) {
		bytes = 0;
		strcpy_s(ctype, "ERR-NFND");
		ptr = (INT_PTR)nullptr;
		return;
	}

	// Pointer arithmetic to return the runtime location within the memory (RAM) that the input
	// variable is stored at.
	for (int i = 0; i < number_shared_globals; i++) {
		if (_stricmp(global, share_global_table[i].name) == 0) {
			INT_PTR base = (INT_PTR)((long)share_global_table[i].base);
			ptr = (INT_PTR)(base + offset);
			return;
		}
	}
	mdd_close(3);
	ptr = (INT_PTR)nullptr;
	return;
}
DWORD rdlData::GetPID(const std::string& processName)
{

	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	Process32First(processesSnapshot, &processInfo);
	
	std::wstring ws(processName.begin(), processName.end());

	if (!ws.compare(processInfo.szExeFile)) {
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo)) {
		if (!ws.compare(processInfo.szExeFile)) {
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

void rdlData::read()
{

	SIZE_T bytesRead;

	if (rdlData::pid == -1) {
		std::cerr << "pid not initialised" << std::endl;
		return;
	}

	if (!strncmp("ERR", ctype, sizeof("ERR"))) {
		return;
	}

	std::lock_guard<std::mutex> lock(s_readProcessLock);
	HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, rdlData::pid);
	if (ReadProcessMemory(processHandle, (LPCVOID)ptr, data, bytes, &bytesRead)) {
		CloseHandle(processHandle);
	}
	else {
		printf("WARNING::Error reading process memory at Address:0x%p\n", ptr);
		return;
	}

}

void rdlData::write(const Buffer& newValue)
{
	if (newValue.GetSize() > bytes) {
		std::cerr << "write overflow to RDL variable:" << name << std::endl;
		return;
	}

	int iResult;

	std::lock_guard<std::mutex> lock(s_readProcessLock);
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	iResult = WriteProcessMemory(processHandle, (LPVOID)ptr, &newValue.GetContents()[0], newValue.GetSize(), NULL);

	CloseHandle(processHandle);
}

rdlData::rdlData()
{

	if (rdlData::pid == -1) {
		GetPID("rtex10.exe");
	}
	s = 0;
	memset(name, '\0', S3_STRLEN);
	memset(global, '\0', S3_STRLEN);
	
	memset(typeDB, '\0', S3_STRLEN);

	memset(vcopt, '\0', S3_STRLEN);
	memset(sdesc,'\0', S3_STRLEN);
	memset(ldesc, '\0', 1024);
	memset(sys_id, '\0', S3_STRLEN);
	memset(fmt, '\0', S3_STRLEN);
	memset(ctype, '\0', S3_STRLEN);



	date = 0;
	offset = 0;
	bytes = 0;
	level = 1; //send 1 for ods only lookup
	for (int i = 0; i < sizeof(dimen) / sizeof(int); i++) {
		dimen[i] = 0;
	}

	lreq = 2;        // #define UO_LEV	2
	ival = 0;
	dval = 0;
	data = nullptr;
}


rdlData::rdlData(const char* vName)
{
	if (rdlData::pid == -1) {
		rdlData::pid =GetPID("rtex10.exe");
	}

	initRDLData(vName);

	if (bytes == 0) {
		if (data != nullptr)
			free(data);
	}
	else {
		char* ptr = (char*)realloc(data, bytes);

		if (ptr != nullptr)
			data = ptr;
		else {
			puts("unable to allocate memory, exiting");
			exit(1);
		}

		read();
	}
}

rdlData::rdlData(const Buffer& vName)
{
	Buffer cpy = vName;

	cpy.nullTerminate();

	if (rdlData::pid == -1) {
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(cpy.GetContents());

	if (bytes == 0) {
		if (data != nullptr)
			free(data);
	}
	else {
		char* ptr = (char*)realloc(data, bytes);

		if (ptr != nullptr)
			data = ptr;
		else {
			puts("unable to allocate memory, exiting");
			exit(1);
		}

		read();
	}
}

void rdlData::init(const Buffer& vName)
{
	Buffer cpy = vName;

	cpy.nullTerminate();

	if (rdlData::pid == -1) {
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(cpy.GetContents());

	if (bytes == 0) {
		if (data != nullptr)
			free(data);
	}
	else {
		char* ptr = (char*)realloc(data, bytes);

		if (ptr != nullptr)
			data = ptr;
		else {
			puts("unable to allocate memory, exiting");
			exit(1);
		}

		read();
	}
}

void rdlData::init(const char* vName)
{
	if (rdlData::pid == -1) {
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(vName);


	if (bytes == 0) {
		if (data != nullptr)
			free(data);
	}
	else {
		char* ptr = (char*)realloc(data, bytes);

		if (ptr != nullptr)
			data = ptr;
		else {
			puts("unable to allocate memory, exiting");
			exit(1);
		}

		read();
	}
}

void rdlData::init(const std::string& vName)
{
	if (rdlData::pid == -1) {
		//todo - process name should be variable
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(vName.c_str());


	if (bytes == 0) {
		if (data != nullptr)
			free(data);
	}
	else {
		char* ptr = (char*)realloc(data, bytes);

		if (ptr != nullptr)
			data = ptr;
		else {
			puts("unable to allocate memory, exiting");
			exit(1);
		}

		read();
	}
}

Buffer rdlData::GetData()
{
	read();
	return Buffer(data, bytes);
}



rdlData::~rdlData()
{
	if (data != nullptr) {
		free(data);
	}
}
