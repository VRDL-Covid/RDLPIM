#include "rdlpch.h"
#include "dataArray.hpp"



void dataArray::deserialise(const Buffer& rawInput)
{
	Buffer input = rawInput;
	Buffer chunk;

	
	while (input.size > 0) {
		chunk = input.PassChunk('{', '}');
		
		auto ptr = CreateRef<DataElement>();
		ptr->deserialise(chunk);
		
		m_DataArry.emplace_back(ptr);
	}
}

dataArray::dataArray()
{

}


dataArray::~dataArray()
{

}
