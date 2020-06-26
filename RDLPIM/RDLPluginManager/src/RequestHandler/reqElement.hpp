#pragma once
#include<stdio.h>
#include<iostream>
#include<string>
#include"buffer.hpp"
#include"RDL/rdlData.hpp"

#define MAXDATASIZE 4096

// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle a single request string from a client formatting:
//				"VarName=VarType=Bytes=Value"
//				passed to request handle objects.
class reqElement
{
private:
	void sizeData(int sBytes);
public:
	//members
	buffer varName;
	buffer type;
	int bytes;
	char* data;

	//methods 

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Set methods overloaded to types required for trasnfer. 
	void set(bool in);
	void set(int in);
	void set(float in);
	void set(double in);
	int set(char* in, int size);
	int set(buffer* in);
	int set(const buffer &in);
	void set(const rdlData& data);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to initialise the contents of a request packet from a correctly formatted character array 
	//				"VarName=VarType=Bytes=Value"
	int deserialise(buffer* in);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to serialise the contents of a request packet into a correctly formatted character array 
	//				"VarName=VarType=Bytes=Value"
	void serialise(buffer* output);


	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to stack build request element, leaves trailing '='
	static int appendData(char* dst, int dSize, char* src, int sSize);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to stack build request element, NO trailing '='
	static int appendValueData(char* dst, int dSize, char* src, int sSize);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Constructor and destructor methods, overloaded to initialise variable name from a string or buffer object.
	reqElement();
	reqElement(std::string var);
	reqElement(const buffer& var);
	reqElement(buffer* var);
	reqElement(const char* var);
	~reqElement();

	//copy constructors

	//copy constructor
	reqElement(const reqElement& other);

	//copy assignment overload
	reqElement& operator=(const reqElement& other);






};

