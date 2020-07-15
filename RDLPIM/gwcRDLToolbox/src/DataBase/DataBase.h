#pragma once
#include"DBElement.h"

class DataBase
{
public:
	~DataBase();

	static DataBase* GetInstance()
	{
		if (s_Instance == nullptr)
			s_Instance = new(DataBase);
		
		return s_Instance;
	}

private://members
	std::unordered_map<std::string, Ref<DBElement>> m_Data;

	static DataBase* s_Instance;
private://methods
	DataBase();
	
	
};