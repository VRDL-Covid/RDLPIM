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

	auto& changeEvent = m_Data[varname_str]->GetOnChangedEvent();
	changeEvent.raiseEvent(data);

	DataElementArray EventParam;
	EventParam.Deserialise(data.Serialise());
}

void DataBase::ModData(const DataElementArray& dataArr)
{
	PROFILE_FUNCTION();
	std::lock_guard<std::mutex> lock(m_DBAccess);
	auto cpy = dataArr;

	for (auto& element : cpy) {
		Buffer varname = element->m_VarName;

		std::string varname_str = varname.ToString();

		if (m_Data.find(varname_str) == m_Data.end()) {
			m_Data[varname_str] = std::move(CreateRef<DBElement>(*element));
			OnNewEntry.raiseEvent(varname_str);
		}
		else {
			m_Data[varname_str]->SetData(*element);
		}

		m_Data[varname_str]->GetOnChangedEvent().raiseEvent(*element);
	}

	OnUpdated.raiseEvent(dataArr);

}

DataElement DataBase::GetData(const std::string& varName)
{
	std::lock_guard<std::mutex> lock(m_DBAccess);
	if (m_Data.find(varName) == m_Data.end()) {
		//not in DB, try RDL as a source.
		DataElement ret(varName);
		ret.set(RDL::Get()->Read(varName.c_str()));
		return ret;
	}
	auto DBE = m_Data[varName];
	return m_Data[varName]->GetData();
}

bool DataBase::PointExists(const std::string& name)
{
	if (m_Data.find(name) != m_Data.end())
		return true;
	return false;
}


DataBase::DataBase()
{
	m_Data.rehash(2000);
}
