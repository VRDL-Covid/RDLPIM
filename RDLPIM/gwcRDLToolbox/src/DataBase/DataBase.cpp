#include"rdlpch.h"
#include"DataBase.h"

DataBase* DataBase::s_Instance = nullptr;

DataBase* DataBase::GetInstance()
{
	{
		if (s_Instance == nullptr)
			s_Instance = new(DataBase);

		return s_Instance;
	}
}

void DataBase::ModData(const DataElement& data)
{
	std::lock_guard<std::mutex> lock(m_DBAccess);
	Buffer varname = data.m_VarName;

	std::string varname_str = varname.ToString();

	if (m_Data.find(varname_str) == m_Data.end()) {
		m_Data[varname_str] = std::move(CreateRef<DBElement>(data));
		OnNewEntry.raiseEvent(varname_str);
	}
	else {
		m_Data[varname_str]->SetData(data);
	}


	m_Data[varname_str]->m_OnChanged.raiseEvent(data);
}

DataElement DataBase::GetData(const std::string& varName)
{
	std::lock_guard<std::mutex> lock(m_DBAccess);
	if (m_Data.find(varName) == m_Data.end()) {
		DataElement ret(varName);
		ret.m_Type = Buffer("NOT_FOUND");
		return ret;
	}
	auto DBE = m_Data[varName];
	return m_Data[varName]->GetData();
}


DataBase::DataBase()
{
	m_Data.rehash(2000);
}
