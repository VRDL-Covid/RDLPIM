#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Core.h"
class DataElementArray
{
public:

	DataElementArray();
	~DataElementArray();

	void Deserialise(const Buffer& rawInput);
	Buffer Serialise();
	Buffer GetVarNames();
	void ClearArray() { m_DataArry.clear(); }
	void AddElement(const DataElement& element) { m_DataArry.push_back(CreateRef<DataElement>(element)); }

	std::vector<Ref<DataElement>>::iterator begin() { return m_DataArry.begin(); }
	std::vector<Ref<DataElement>>::iterator end() { return m_DataArry.end(); }

	//move opertor
	DataElementArray& operator=(DataElementArray&& other);


	//copy opertator
	DataElementArray& operator=(const DataElementArray& other);
	DataElementArray(const DataElementArray& other);


private:
	std::vector<Ref<DataElement>> m_DataArry;
};

