#include "rdlpch.h"
#include "rdlData.hpp"
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


DWORD rdlData::pid = -1;

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
			ptr = (INT_PTR)((long)share_global_table[i].base + offset);
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
	const int bufferSize = 8;
	char* buffer[bufferSize];
	SIZE_T bytesRead;
	double retVar;

	if (rdlData::pid == -1) {
		std::cerr << "pid not initialised" << std::endl;
		return;
	}

	if (!strncmp("ERR-NFND", ctype, sizeof("ERR-NFND"))) {
		return;
	}

	HANDLE processHandle = OpenProcess(PROCESS_VM_READ, false, rdlData::pid);

	if (ReadProcessMemory(processHandle, (LPCVOID)ptr, buffer, bufferSize, &bytesRead)) {
		CloseHandle(processHandle);
	}
	else {
		printf("WARNING::Error reading process memory at Address:0x%p\n", ptr);
		return;
	}

	memcpy(value, &buffer, 8);
}

void rdlData::write(const buffer& newValue)
{
	if (newValue.size > bytes) {
		std::cerr << "write overflow to RDL variable:" << name << std::endl;
		return;
	}

	int iResult;

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	iResult = WriteProcessMemory(processHandle, (LPVOID)ptr, &newValue.contents[0], newValue.size, NULL);

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
	memset(sdesc, '\0', S3_STRLEN);
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
	value = nullptr;
}


rdlData::rdlData(const char* vName)
{
	if (rdlData::pid == -1) {
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(vName);


	value = (char*)malloc(bytes);
	read();
}

rdlData::rdlData(const buffer& vName)
{
	buffer cpy = vName;

	cpy.nullTerminate();

	if (rdlData::pid == -1) {
		rdlData::pid = GetPID("rtex10.exe");
	}

	initRDLData(cpy.contents);

	value = (char*)malloc(bytes);
	read();
}


rdlData::~rdlData()
{
	if (value != nullptr) {
		free(value);
	}
}
