#pragma once
#include"buffer.hpp"
#include<windows.h>
#include<tlhelp32.h>
#include"s3Includes.hpp"
#include<stdio.h>
#include<iostream>

// type to convert types prior to sending over TCP/IP using char array buffer.

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
	void write(const buffer& newValue);
	void init(const buffer& vName);
	void init(const char* vName);

	rdlData();
	rdlData(const char* vName);
	rdlData(const buffer& vName);
	~rdlData();
};
