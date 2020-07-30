#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Event.h"

class DBElement
{
public:
	DBElement(const DataElement& de):m_Data(de), m_RDLOwned(false){};
	~DBElement() = default;;

	const DataElement& GetData()const { return m_Data; }
	void SetData(const DataElement& data) { m_Data = data; m_OnChanged.raiseEvent(m_Data); }
	void SetData(const rdlData& data) { m_Data.set(data); m_RDLOwned = true; m_OnChanged.raiseEvent(m_Data); }

	const bool IsRDLOwned()const { return m_RDLOwned; }
	void SetRDLOwned(bool state) { m_RDLOwned = state; }

	Event<const DataElement&>& GetOnChangedEvent() { return m_OnChanged; }

private:
	DataElement m_Data;
	bool m_RDLOwned;
	Event<const DataElement&> m_OnChanged;
};
