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
		//todo PassDataElement;
		chunk = GetSerialisedDataElement(input);
		
		auto ptr = CreateRef<DataElement>();
		ptr->deserialise(chunk);
		
		m_DataArry.emplace_back(ptr);
	}
}


Buffer DataElementArray::GetSerialisedDataElement(Buffer& rawInput)
{
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t bytes = 0;
	Buffer out;

	//find firt delimiter
	for (int i = 0; i < rawInput.GetSize(); i++) {
		if (rawInput.GetContents()[i] != '{')
			++start;
		else
			break;
	}

	//find second delimiter

	int equCount = 0;
	int it = 0;

	while (equCount < 2) {
		++it;

		if (rawInput.GetContents()[start + it] == '=') {
			++equCount;
		}
	}

	int remBytes = *(int*)(&rawInput.GetContents()[start + it + 1]);

	end = start + it + 5 + remBytes;


	bytes = end - start + 1;

	char* temp = (char*)malloc(bytes);

	for (uint32_t i = 0; i < bytes; i++) {
		temp[i] = rawInput.GetContents()[i + start];
	}

	out.set(temp, bytes);

	free(temp);

	rawInput.stripHead((int)bytes);
	out.stripHead('{');
	out.stripTail('}');
	return out;

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
			retBuff.append(element->GetName());
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