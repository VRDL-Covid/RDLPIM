#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Event.h"

class DBElement
{
public:
	DBElement(const DataElement& de):m_Data(de){};
	~DBElement() = default;;

	const DataElement& GetData()const { return m_Data; }
	void SetData(const DataElement& data) { m_Data = data; m_OnChanged.raiseEvent(m_Data); }
	void SetDataSilent(const DataElement& data) { m_Data = data; }

	Event<const DataElement&>& GetOnChangedEvent() { return m_OnChanged; }

private:
	DataElement m_Data;
	Event<const DataElement&> m_OnChanged;
};
