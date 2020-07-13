#pragma once
#include"reqElement.hpp"
class dataArray
{
public:
	std::vector<responceElement*> data;

	int nElements;
	void deserialise(const buffer& rawInput);
	dataArray();
	~dataArray();
};

