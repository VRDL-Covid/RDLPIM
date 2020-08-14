#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Core.h"

/// @brief A container class to serialise and deserialise an array of DataElements to. Required by the DataBase::ModData(const DataElementArray& dataArr) method.
class DataElementArray
{
public:

	DataElementArray();
	~DataElementArray();

	/// @brief Deserialise a Buffer object into an array of DataElements
	/// @param rawInput Buffer object containing serialised data.
	void Deserialise(const Buffer& rawInput);

	/// @brief Serialise a DataElementArray object into a Buffer
	/// @return Buffer of serialised data
	Buffer Serialise();

	/// @brief A method to return a {} delimieted list of the variable names within the array of DataElements
	/// @return {} delimieted list of the variable names within the array of DataElements
	Buffer GetVarNames();

	/// @brief A method to empty the internally stored DataElement array
	void ClearArray() { m_DataArry.clear(); }

	/// @brief A method to append a DataElement object to the DataElement array stored within this object
	/// 
	/// This is a utility function to help build an array of DataElements to be serialised.
	/// @param element DataElement object to be placed into the array
	void AddElement(const DataElement& element) { m_DataArry.push_back(CreateRef<DataElement>(element)); }

	/// @brief Custom for loop iteration
	/// @return iterator
	std::vector<Ref<DataElement>>::iterator begin() { return m_DataArry.begin(); }

	/// @brief Custom for loop iteration
	/// @return iterator
	std::vector<Ref<DataElement>>::iterator end() { return m_DataArry.end(); }

	/// @brief Move operator overload
	/// @param other RHS
	/// @return DataElementArray reference
	DataElementArray& operator=(DataElementArray&& other);


	/// @brief Copy assignment operator
	/// @param other RHS
	/// @return DataElementArray reference
	DataElementArray& operator=(const DataElementArray& other);

	/// @brief Copy Constructor
	/// @param other RHS
	DataElementArray(const DataElementArray& other);


private:
	std::vector<Ref<DataElement>> m_DataArry;
};

