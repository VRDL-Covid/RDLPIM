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
	Buffer varname = data.m_VarName;

	if (m_Data.find(varname.ToString()) == m_Data.end()) {
		m_Data[varname.ToString()] = std::move(CreateRef<DBElement>(data));
	}
	else {
		m_Data[varname.ToString()]->SetData(data);
	}


	m_Data[varname.ToString()]->m_OnChanged.raiseEvent(data);
}

DataBase::DataBase()
{
	m_Data.rehash(512);
}
