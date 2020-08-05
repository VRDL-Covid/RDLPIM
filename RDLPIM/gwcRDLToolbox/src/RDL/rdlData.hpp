#pragma once
#include"Core/buffer.hpp"
#include<windows.h>
#include<tlhelp32.h>
#include"s3Includes.hpp"

// type to convert types prior to sending over TCP/IP using char array buffer.

extern "C" {
	// imported functions from s3dll.dll
	typedef void(__stdcall* __ET)();
	typedef struct
	{
		__ET entry;
		char* longname;
		char* user;
		char	type, cons, dummy, level;
		int	argc;
		unsigned int mflags;
	} MODTAB;

	__declspec(dllimport) int mdd_open(char* who);
	__declspec(dllimport) void mdd_close(int which);
	__declspec(dllimport) int DbmPidNew(int s, char* instring, int lreq, char* typedb,
										char* global, int* offset, char* type1, int* prec, int* dimen, char* vcopt,
										long* ival, double* dval, int* lfound, char* sdesc, char* ldesc, char* unit,
										char* sys_id, char* fmt, unsigned int* date);

	// imported data from mstg.dll
	__declspec(dllimport) int number_shared_globals;
	__declspec(dllimport) GLOBAL_TABLE share_global_table[];

};

class rdlData
{
private:
	DWORD GetPID(const std::string& processName);
	void initRDLData(const char* varname);
public:
	int s = 0;
	char name[S3_STRLEN];
	char global[S3_STRLEN];
	char typeDB[S3_STRLEN];
	char vcopt[S3_STRLEN];
	char sdesc[S3_STRLEN];
	char ldesc[1024];
	char unit[S3_STRLEN];
	char sys_id[S3_STRLEN];
	char fmt[S3_STRLEN];
	unsigned int date;
	int offset;
	char ctype[S3_STRLEN];
	int bytes = 0;
	int level = 1;         //send 1 for ods only lookup
	int dimen[7];
	int lreq = 2;        // #define UO_LEV	2
	long ival;
	double dval;

	INT_PTR ptr;
	static DWORD pid;
	char* data;


	void read();
	void write(const Buffer& newValue);
	void init(const Buffer& vName);
	void init(const char* vName);
	void init(const std::string& vName);

	const std::string GetName()const { return std::string(name); }

	rdlData();
	rdlData(const char* vName);
	rdlData(const Buffer& vName);
	~rdlData();
};
