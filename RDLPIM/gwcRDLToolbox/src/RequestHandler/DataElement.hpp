#pragma once
#include"Core/buffer.hpp"
#include"RDL/rdlData.hpp"

// Author: Guy Collins
// Date: 20/01/2020
// Description: A class to handle a single request string from a client formatting:
//				"VarName=VarType=Bytes=Value"
//				passed to request handle objects.


/// @brief A class to handle the data transfer of complex and primitive objects into and out of the DataBase instance
/// 
/// the serialised format is as follows:
/// 
///   {[char]=[char]=[int][char]}
/// 
/// "{VarName=VarType=Bytes Value}"
/// (no space delimeter between Bytes and Value since Bytes is always 4Bytes in size and describes the length of Value)
/// 
/// Overloaded functions have been provided to allow primitives to be injected into DataElements readily
/// ###Example
/// ~~~~~~~~~~.cpp
/// DataElement de("varName");
/// double value;
/// Buffer serialised;
/// 
/// value = 3.14159;
/// de.set(value);
/// 
/// serialised = de.Serialise();
/// 
/// serialised.fullPrint();
///	//				  { int = 8 bytes    } {     binary for 3.14159 as double      }
/// //"{varName=double=0x08 0x00 0x00 0x00   0x40 0x09 0x21 0xF9 0xF0 0x1B 0x86 0x6E}"
/// 
/// 
/// ~~~~~~~~~~
class DataElement
{
private:
	void sizeData(int sBytes);

	//members
	Buffer m_VarName;
	Buffer m_Type;
	int m_Bytes;
	char* m_data;
public:



	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Constructor and destructor methods, overloaded to initialise variable name from a string or buffer object.
	DataElement();
	DataElement(std::string var);
	DataElement(const Buffer& var);
	DataElement(Buffer* var);
	DataElement(const char* var);
	DataElement(const rdlData& var);
	~DataElement();


	/// @brief Set DataElement to contain boolean primitive
	/// @param in value
	void set(bool in);

	/// @brief Set DataElement to contain integer primitive
	/// @param in value
	void set(int in); 

	/// @brief Set DataElement to contain float primitive
	/// @param in value
	void set(float in);

	/// @brief Set DataElement to contain double primitive
	/// @param in value
	void set(double in);

	/// @brief Set DataElement to contain a blob/serialised datatype
	/// @param in value
	int set(char* in, int size); 

	/// @brief Set DataElement to contain a blob/serialised datatype
	/// @param in value
	int set(Buffer* in); 

	/// @brief Set DataElement to contain a blob/serialised datatype
	/// @param in value
	int set(const Buffer &in);

	/// @brief Set DataElement to contain a RDL data type, type is deduced from rdlData object
	/// @param data source data
	void set(const rdlData& data); 

	/// @brief A method to retrieve the name of the variable contained within the DataElement
	/// @return Buffer representation of the name
	const Buffer GetName() const { return m_VarName; }

	/// @brief A method to retrieve the type of the variable contained within the DataElement
	/// @return string representation of the type	
	const std::string GetType()const;

	/// @brief A method to retrieve the type of the variable contained within the DataElement
	/// @return string representation of the type	
	const int GetSize() const { return m_Bytes; }

	/// @brief A method to return a copy of the raw data contained within the DataElement
	/// @return raw binary as a char array
	char* GetData() const { return m_data; }

	/// @brief A method used to manually overider the data type stored within the DataElement
	/// 
	/// This is required to initialise a DataElement as 'INIT' for the RDL to populate
	/// @param type Buffer representation of the type eg Buffer("INIT")
	void SetType(const Buffer& type) { m_Type = type; }

	/// @brief A method to deserialise back into a DataElement object
	/// @param in serialised data
	/// @return An integer of the number of components succesfully deserialised 1 name 2 name and type, 3 name type and size, 4 name type size and data.
	int deserialise(const Buffer& in);

	/// @brief Method to return the name of the variable compatible with a pull data stack '{varName}'
	/// @return Buffer of serialised name
	Buffer SerialiseName();
	
	// Author: Guy Collins
	// Date: 20/01/2020
	// Description: Method to serialise the contents of a request packet into a correctly formatted character array 
	//				"{VarName=VarType=Bytes=Value}"

	/// @brief Method to serialise the contents of a request packet into a correctly formatted character array 
	/// 
	/// "{VarName=VarType=Bytes=Value}"
	/// @warning serialisation will add { } top and tail to facilitate stacking of DataElements.
	/// @return Buffer of serialised component
	Buffer Serialise()const;

	/// @brief Copy constructor
	/// @param other RHS
	DataElement(const DataElement& other);

	/// @brief Move constructor
	/// @param other RHS
	/// @return 
	DataElement(DataElement&& other) noexcept;

	/// @brief Copy assignment operator overload
	/// @param other RHS
	/// @return DataElement reference
	DataElement& operator=(const DataElement& other);

	/// @brief Move assignment operator overload
	/// @param other R-Value reference of DataElement object (temp object)
	/// @return DataElement reference
	DataElement& operator=(DataElement&& other) noexcept;

};

