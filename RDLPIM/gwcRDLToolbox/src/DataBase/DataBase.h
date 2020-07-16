#pragma once
#include"DBElement.h"

class DataBase
{
public:
	~DataBase() = default;

	static DataBase* GetInstance();

	void ModData(const DataElement& data);
	//void RemoveData(const DataElement& data);
	
	DataElement GetData(const std::string& varname);

	Event<const std::string>& GetOnNewEntry() { return OnNewEntry; }



private://members
	//std::unordered_map<Buffer, Ref<DBElement>> m_Data;
	std::unordered_map<std::string, Ref<DBElement>> m_Data;

	static DataBase* s_Instance;
private://methods
	DataBase();
	
	std::mutex m_DBAccess;

	Event<const std::string> OnNewEntry;
};