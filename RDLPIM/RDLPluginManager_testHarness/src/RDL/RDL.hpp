#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include"buffer.hpp"
#include"s3Includes.hpp"

//buff length and port
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "502"
#define NUM_NODES 100
#define MAX_COILS 2000
#define MAX_IO 50

//Define boolean shortcut
#define on 0xFF
#define off 0x00

//MasterDigital out data array structure
struct MasterTypeArr {
	char name[32];
	int address;
	char node_name[50];
};

//Digital out data array structure
struct TypeDOIArr {
	char name[32];
	int coil;
	bool state;
	char sw[32];
};

//int in/out data array structure
struct TypeIArr {
	char name[32];
	int address;
	int Int;
};

//float in/out data array structure
struct TypeFArr {
	char name[32];
	int address;
	float Float;
};

//Double in/out data array structure
struct TypeRArr {
	char name[32];
	int address;
	double Double;
};


// type to convert types prior to sending over TCP/IP using char array buffer.
union convertable {
	char Char[8];
	double Double;
	float Float;
	int Int;
	bool Bool;
};
/*
struct TypeNodeConfig {
	char name[50];
	char ip[15];
	char port[5];
	bool isActive;
	SOCKET socket;
};

struct TypeConnection {
	SOCKET socket;
	char name[50];
};
*/
//Configuration file locations
#define NodeConfig "Node_config.txt"
#define DOConfig "do_config.txt"
#define DIConfig "di_config.txt"
#define IOConfig "io_config.txt"
#define IIConfig "ii_config.txt"
#define FOConfig "fo_config.txt"
#define FIConfig "fi_config.txt"
#define ROConfig "ro_config.txt"
#define RIConfig "ri_config.txt"

//Define buffer size in bytes to temp store register data.
#define BUF_SIZE 512


// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle interaction with the RDL, constructed by passing a buffer* / char* containing the processes name eg "rtex10"
class RDL
{
public:
	DWORD pid;
	RDL();
	RDL(const buffer &ipid);
	RDL(const char* ipid);
	RDL(char* ipid);
	~RDL();

	// plcGetVarAddress:
	// input as character array
	// Function:  Return the runtime memory location of a variable managed by DBM, eg "th_temp_eff"
	long plcGetVarAddress(char* varname);


	// readMemDouble
	// Read double variable from memory space
	// specify the hex address within the memory that you would like to read
	// and the PID that the memory has been allocated to. Required under Windows OS.
	double readMemDouble(INT_PTR address);


	// readMemInt
	// Read integer variable from memory space
	// specify the hex address within the memory that you would like to read
	// and the PID that the memory has been allocated to. Required under Windows OS.
	int readMemInt(INT_PTR address);

	// readMemBool
	// Read Boolean variable from memory space
	// specify the hex address within the memory that you would like to read
	// and the PID that the memory has been allocated to. Required under Windows OS.
	bool readMemBool(INT_PTR address);

	//RDL_Active deteremins if the RDL is still running.
	bool RDL_Active();

	// readDouble
	// Read double variable from RDL
	double readDouble(char* varname);
	double readDouble(const char* varname);


	// readMemInt
	// Read integer from RDL
	int readInt(char* varname);
	int readInt(const char* varname);


	// readMemBool
	// Read Boolean from RDL
	bool readBool(char* varname);
	bool readBool(const char* varname);

	// WriteMem
	// Write value to memory space
	// specify the hex address within the memory that you would like to read
	// and the PID that the memory has been allocated to. Required under Windows OS.
	// this function has been overloaded for multiple types.
	void WriteMem(INT_PTR address, bool value);
	void WriteMem(INT_PTR address, int value);
	void WriteMem(INT_PTR address, double value);


	//TODO:GWC - comment
	//void initRDLData(buffer& varname, rdlData& inData);
	//void initRDLData(const char* varname, rdlData& inData);

};