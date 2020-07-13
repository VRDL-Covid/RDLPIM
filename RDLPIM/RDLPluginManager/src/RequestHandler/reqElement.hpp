#pragma once
#include"buffer.hpp"
#include"RDL/rdlData.hpp"

#define MAXDATASIZE 4096

// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle a single request string from a client formatting:
//				"VarName=VarType=Bytes=Value"
//				passed to request handle objects.
class responceElement
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
	void set(bool in); //bool
	void set(int in); //bool
	void set(float in); //bool
	void set(double in); //bool
	int set(char* in, int size); //blob
	int set(buffer* in); //bool
	int set(const buffer &in); //bool
	void set(const rdlData& data); //bool

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
	buffer serialise();


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
	responceElement();
	responceElement(std::string var);
	responceElement(const buffer& var);
	responceElement(buffer* var);
	responceElement(const char* var);
	~responceElement();

	//copy constructors

	//copy constructor
	responceElement(const responceElement& other);

	//copy assignment overload
	responceElement& operator=(const responceElement& other);






};

