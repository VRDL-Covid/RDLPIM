#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Core.h"
class dataArray
{
public:

	void deserialise(const Buffer& rawInput);
	dataArray();
	~dataArray();

	std::vector<Ref<DataElement>>::iterator begin() { return m_DataArry.begin(); }
	std::vector<Ref<DataElement>>::iterator end() { return m_DataArry.end(); }

private:
	std::vector<Ref<DataElement>> m_DataArry;
};

