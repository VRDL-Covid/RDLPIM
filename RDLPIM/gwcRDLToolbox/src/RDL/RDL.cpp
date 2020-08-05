#include "rdlpch.h"
#include "RDL.hpp"

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

RDL::~RDL()
{
}

RDL* RDL::Get()
{
	if (s_Instance == nullptr)
		s_Instance = new RDL;
	return s_Instance;
}

void RDL::worker(bool& work)
{
	auto trkedCpy = m_trackedVars;
	auto DB = DataBase::GetInstance();
	DataElementArray dataArr;
	rdlData reader;

	while (work) {
		if (RDL_Active()) {
			{
				std::lock_guard<std::mutex> lock(m_TrackedVarsArray);
				trkedCpy = m_trackedVars;
			}

			for (auto var : trkedCpy) {
				reader.init(var.first);
				if (reader.GetData() != var.second) { //if rdlData has changed.
					dataArr.AddElement(DataElement(reader));
				}
			}

			DB->ModData(dataArr);
			dataArr.ClearArray();
			
			//todo- Sleep is not cross platform, should use chrono
			Sleep(250);
		}
		else {
			//Todo - process name should be a variable
			Init("rtex10.exe");

			//todo- Sleep is not cross platform, should use chrono
			Sleep(2000);
		}
	}
}

rdlData RDL::Read(const char* varname)
{
	return rdlData(varname);
}

rdlData RDL::Read(char* varname)
{
	return rdlData(varname);
}

void RDL::Write(const char* varName, char* data, size_t size)
{
	char _name[S3_STRLEN];
	strcpy_s(_name,(rsize_t)S3_STRLEN, varName);

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	WriteProcessMemory(processHandle, (LPVOID)FindRuntimePointer(_name), data, size, NULL);
}

void RDL::Write(const std::string& varName, char* data, size_t size)
{
	char _name[S3_STRLEN];
	strcpy_s(_name, (rsize_t)S3_STRLEN, varName.c_str());

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	WriteProcessMemory(processHandle, (LPVOID)FindRuntimePointer(_name), data, size, NULL);
}


void RDL::Write(const DataElement& data)
{

	char _name[S3_STRLEN];
	strcpy_s(_name,(rsize_t)S3_STRLEN, data.GetName().c_str());

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	if (data.GetType() == std::string("bool")) {
		bool* status;
		status = (bool*)data.m_data;
		Write<bool>(data.GetName(), *status);
		return;
	}

	WriteProcessMemory(processHandle, (LPVOID)FindRuntimePointer(_name), data.m_data, data.m_Bytes, NULL);
}


void RDL::TrackVariable(const rdlData& variable)
{
	std::lock_guard<std::mutex> lock(m_TrackedVarsArray);

	uint32_t count = 0;

	if (m_trackedVars.find(variable.GetName()) == m_trackedVars.end()) 
		m_trackedVars.emplace(variable.GetName(), Buffer(variable.data, variable.bytes));
	

	auto& DB_entry = DataBase::GetInstance()->GetEntry(variable.GetName());
	DB_entry->GetOnChangedEvent().subscribe(c_DB_ElementChanged);
}

void RDL::UntrackVariable(const std::string& varName)
{
	std::lock_guard<std::mutex> lock(m_TrackedVarsArray);
	auto& DB_entry = DataBase::GetInstance()->GetEntry(varName);
	DB_entry->GetOnChangedEvent().unsubscribe(c_DB_ElementChanged);

	for (auto it = m_trackedVars.begin(); it != m_trackedVars.end(); it++) {
		if (it->first == varName) {
			m_trackedVars.erase(it);
			break;
		}
	}
}


void RDL::Init(const char* processName)
{
	pid = GetPID(processName);
}

// plcGetVarAddress:
// input as character array
// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
long RDL::FindRuntimePointer(char* varname) {
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
	if (pid == 0)
		return false;

	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}

bool RDL::OnNewVariableHandler(const std::string& varName)
{
	auto& DB_entry = DataBase::GetInstance()->GetEntry(varName);
	
	if (RDL_Active()) {
		rdlData tmp(varName.c_str());
		if (std::string(tmp.ctype) != std::string("ERR-NFND")) { // if RDL owns it
			DB_entry->SetRDLOwned(true);
			if (DB_entry->GetData().GetType() == std::string("INIT")) { //if doesnt contain data (un-initialised subscribe)
				DB_entry->SetData(tmp);
			}
			else {														//if contains data RDL data push
				//set RDL to data
				auto& element = DB_entry->GetData();
				Write(element);
			}
			TrackVariable(tmp);
		}
		else { //If RDL doesnt own it
			if (DB_entry->GetData().GetType() == std::string("INIT")) { //last chance initialise failed
				DB_entry->SetData(tmp); //set to err not found
			}
		}
	}
	else {
		if (DB_entry->GetData().GetType() == std::string("INIT")) { //if doesnt contain data (un-initialised subscribe)
			DataElement errData(varName);
			DB_entry->SetData(errData);
		}
	}
	
	return PROPAGATE_EVENT;
}

bool RDL::OnDataElementChanged(const DataElement& data)
{

	Write(data);
	return PROPAGATE_EVENT;
}


//Private implimentations
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
