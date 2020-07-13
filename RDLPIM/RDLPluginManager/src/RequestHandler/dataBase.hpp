#pragma once
#include"buffer.hpp"
#include"dataElement.hpp"

class dataBase
{
private:
	static dataBase* instance;
public:
	
	static dataBase* getInstance();

	std::vector<dataElement> points;

	int findElement(const buffer& name);

	dataBase();
	~dataBase();
};

