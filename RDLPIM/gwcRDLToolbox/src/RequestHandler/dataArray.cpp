#include "rdlpch.h"
#include "dataArray.hpp"

DataElementArray::DataElementArray()
{

}


DataElementArray::~DataElementArray()
{

}


void DataElementArray::Deserialise(const Buffer& rawInput)
{
	PROFILE_FUNCTION();
	Buffer input = rawInput;
	Buffer chunk;

	
	while (input.GetSize() > 0) {
		chunk = input.PassChunk('{', '}');
		
		auto ptr = CreateRef<DataElement>();
		ptr->deserialise(chunk);
		
		m_DataArry.emplace_back(ptr);
	}
}


Buffer DataElementArray::Serialise()
{
	Buffer retBuff;

	if (m_DataArry.size() > 0) {
		for (auto& element : *this) {
			retBuff.append(element->Serialise());
		}
	}
	else {
		retBuff.set("{}");
	}

	return retBuff;
}


Buffer DataElementArray::GetVarNames()
{
	Buffer retBuff;

	if (m_DataArry.size() > 0) {
		for (auto& element : *this) {
			retBuff.append("{");
			retBuff.append(element->m_VarName);
			retBuff.append("}");
		}
	} 
	else {
		retBuff.set("{}");
	}

	return retBuff;
}

//move operator
DataElementArray& DataElementArray::operator=(DataElementArray&& other)
{
	this->m_DataArry = std::move(other.m_DataArry);

	return *this;
}

//copy
DataElementArray& DataElementArray::operator=(const DataElementArray& other)
{
	this->m_DataArry = other.m_DataArry;

	return *this;
}

//copy
DataElementArray::DataElementArray(const DataElementArray& other)
{
	this->m_DataArry = other.m_DataArry;
}