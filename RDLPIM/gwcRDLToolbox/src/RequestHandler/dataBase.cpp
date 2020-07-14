#include "rdlpch.h"
#include "dataBase.hpp"

dataBase* dataBase::instance = nullptr;

dataBase* dataBase::getInstance()
{
	if (instance == nullptr)
		instance = new dataBase;
	return instance;
}


int dataBase::findElement(const buffer& name)
{

	buffer t_name = name;
	int result = -1;
	
	std::vector<dataElement>::iterator it = points.begin();

	for (it; it!=points.end(); it++) {
		if (t_name == it->varName) {
			return std::distance(points.begin(),it);
		}
	}

	return result;
}


dataBase::dataBase()
{
	int a = 5;
}


dataBase::~dataBase()
{

}
