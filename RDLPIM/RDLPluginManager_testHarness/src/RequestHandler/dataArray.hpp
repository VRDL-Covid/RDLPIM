#pragma once
#include"reqElement.hpp"
class dataArray
{
public:
	std::vector<reqElement*> data;

	int nElements;
	void deserialise(const buffer& rawInput);
	dataArray();
	~dataArray();
};

