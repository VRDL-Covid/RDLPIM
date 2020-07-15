#pragma once
#include"DBElement.h"

class DataBase
{
public:
	~DataBase() = default;

	static DataBase* GetInstance();

	void ModData(const DataElement& data);
	//void RemoveData(const DataElement& data);
	
	//const DataElement& GetData(const std::string& varname);

private://members
	//std::unordered_map<Buffer, Ref<DBElement>> m_Data;
	std::unordered_map<std::string, Ref<DBElement>> m_Data;

	static DataBase* s_Instance;
private://methods
	DataBase();
	
	
};