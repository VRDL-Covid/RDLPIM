#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Core.h"
class dataArray
{
public:

	void deserialise(const buffer& rawInput);
	dataArray();
	~dataArray();

private:
	std::vector<Ref<DataElement>> m_DataArry;
};

