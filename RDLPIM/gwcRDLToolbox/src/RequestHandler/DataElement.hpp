#pragma once
#include"Core/buffer.hpp"
#include"RDL/rdlData.hpp"

// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle a single request string from a client formatting:
//				"VarName=VarType=Bytes=Value"
//				passed to request handle objects.
class DataElement
{
private:
	void sizeData(int sBytes);
public:
	//members
	buffer m_VarName;
	buffer m_Type;
	int m_Bytes;
	char* m_data;

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
	//				"{VarName=VarType=Bytes=Value}"
	int deserialise(const buffer& in);

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to serialise the contents of a request packet into a correctly formatted character array 
	//				"{VarName=VarType=Bytes=Value}"
	buffer serialise();

	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Constructor and destructor methods, overloaded to initialise variable name from a string or buffer object.
	DataElement();
	DataElement(std::string var);
	DataElement(const buffer& var);
	DataElement(buffer* var);
	DataElement(const char* var);
	~DataElement();

	//copy constructors

	//copy constructor
	DataElement(const DataElement& other);

	//copy assignment overload
	DataElement& operator=(const DataElement& other);






};

