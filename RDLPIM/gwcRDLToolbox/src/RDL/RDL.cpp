#include "rdlpch.h"
#include "RDL.hpp"

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
DWORD GetPID(const std::string& processName) {

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


RDL* RDL::s_Instance = nullptr;

RDL::RDL()
{
	//initialise RDL process ID
	pid = 0;
	
	//subscribe to new data elements in the RDLPIM database.
	c_NewDataEntry = CreateRef<EventCallback<const std::string&>>();
	c_NewDataEntry->SetCallback(BIND_EVENT_FN(RDL::OnNewVariableHandler));
	DataBase::GetInstance()->GetOnNewEntry().subscribe(c_NewDataEntry);
	
	//build on DataElementChanged callback object
	c_DB_ElementChanged = CreateRef<EventCallback<const DataElement&>>();
	c_DB_ElementChanged->SetCallback(BIND_EVENT_FN(RDL::OnDataElementChanged));
}

RDL* RDL::Get()
{
	if (s_Instance == nullptr)
		s_Instance = new RDL;
	return s_Instance;
}

void RDL::Init(const char* processName)
{
	pid = GetPID(processName);
}

RDL::~RDL()
{
}

// plcGetVarAddress:
// input as character array
// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
long RDL::plcGetVarAddress(char* varname) {
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
	//TODO - GWC, do i really want to mdd_open every plcGetVar address call put these in the constructor/destructor and test onsite!
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

bool RDL::OnNewVariableHandler(const std::string& varName)
{
	//Todo, handle differently if RDL is not active. so not to invoke mdd_open.
	rdlData tmp(varName.c_str());
	auto& DB_entry = DataBase::GetInstance()->GetEntry(varName);

	if (std::string(tmp.ctype) != std::string("ERR-NFND")) { // if RDL owns it
		DB_entry->SetRDLOwned(true);
		if (DB_entry->GetData().GetType() == std::string("INIT")) { //if doesnt contain data (un-initialised subscribe)
			DB_entry->SetData(tmp);
		}
		else {														//if contains data RDL data push
			//set RDL to data
			auto& element = DB_entry->GetData(); 
			Write(element.m_VarName.ToString().c_str(), element.m_data, (size_t)element.m_Bytes);
		}
		TrackVariable(varName);
	}
	else { //If RDL doesnt own it
		if (DB_entry->GetData().GetType() == std::string("INIT")) { //last chane initialise failed
			DB_entry->SetData(tmp); //set to err not found
		}
	}

	return PROPAGATE_EVENT;
}

void RDL::TrackVariable(const std::string& varName)
{
	std::lock_guard<std::mutex> lock(m_TrackedVarsArray);

	uint32_t count = 0;

	for (std::string var : m_trackedVars) {
		if (var == varName)
			++count;
	}

	if (!count)
		m_trackedVars.push_back(varName);

	auto& DB_entry = DataBase::GetInstance()->GetEntry(varName);
	DB_entry->GetOnChangedEvent().subscribe(c_DB_ElementChanged);
}

void RDL::UntrackVariable(const std::string& varName)
{
	std::lock_guard<std::mutex> lock(m_TrackedVarsArray);
	auto& DB_entry = DataBase::GetInstance()->GetEntry(varName);
	DB_entry->GetOnChangedEvent().unsubscribe(c_DB_ElementChanged);

	for (auto it = m_trackedVars.begin(); it != m_trackedVars.end(); it++) {
		if (*(it) == varName) {
			m_trackedVars.erase(it);
			break;
		}
	}
}

bool RDL::OnDataElementChanged(const DataElement& data)
{
	Write(data.m_VarName.ToString().c_str(),data.m_data, (size_t)data.m_Bytes);

	return PROPAGATE_EVENT;
}