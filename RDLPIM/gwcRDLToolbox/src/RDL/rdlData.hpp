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


/// @brief A RDL runtime data catcher object, will execute a DBM lookup to determine the 'RDL attributes' of the datagram. A pointer to the process memory is developed enabling the realtime value to be obtained.
///  
class rdlData
{
private:
	DWORD GetPID(const std::string& processName);
	void initRDLData(const char* varname);
public:
	int s = 0; ///<unknown property
	char name[S3_STRLEN]; ///<RDL variable name
	char global[S3_STRLEN];///<Global memory partition name eg Global_V
	char typeDB[S3_STRLEN]; ///<unknown property
	char vcopt[S3_STRLEN]; ///<unknown property
	char sdesc[S3_STRLEN];///<Short Description of variable
	char ldesc[1024];///<Long description of variable
	char unit[S3_STRLEN];///<Engineering units
	char sys_id[S3_STRLEN];///<system ID eg CM (coolant makeup)
	char fmt[S3_STRLEN];///<console write formate, eg %f
	unsigned int date; ///<Date added/modified
	int offset; ///<offset from base memory location of the owning global partition
	char ctype[S3_STRLEN]; ///<DataType 
	int bytes = 0; ///<size of data
	int level = 1; ///<send 1 for ods only lookup
	int dimen[7]; ///<Array dimension 0 = single primitive, 1= 1D array etc
	int lreq = 2; ///< #define UO_LEV	2
	long ival; ///<initial value
	double dval; ///<default value

	INT_PTR ptr; ///<runtime memory location
	static DWORD pid; ///<process ID of the RDL (static)
	static std::mutex s_readProcessLock; ///<thread safety process read
	char* data;///< contents of realtime memory location at last read() execution

	/// @brief A method to flush the RDL's process memory for into this object
	void read();

	/// @brief A method to write the raw contents of a Buffer into the RDL's process memory
	/// @param newValue raw data
	void write(const Buffer& newValue);

	/// @brief A method to initialise and couple the rdlData object to the RDL process
	/// @param vName RDL variable name.
	void init(const Buffer& vName);

	/// @brief A method to initialise and couple the rdlData object to the RDL process
	/// @param vName RDL variable name.
	void init(const char* vName);

	/// @brief A method to initialise and couple the rdlData object to the RDL process
	/// @param vName RDL variable name.
	void init(const std::string& vName);

	/// @brief A method to return the variable name that this rdlData object is responcible for
	/// @return std::string representation of the variable name.
	const std::string GetName()const { return std::string(name); }

	/// @brief A method to return a copy of the RDL's process memory as a Buffer object
	/// @return raw process memory stored in a Buffer object
	Buffer GetData();

	rdlData();
	rdlData(const char* vName);
	rdlData(const Buffer& vName);
	~rdlData();
};
