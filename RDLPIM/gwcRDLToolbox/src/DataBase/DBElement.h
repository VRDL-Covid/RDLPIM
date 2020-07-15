#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Event.h"

class DBElement
{
public:
	DBElement(DataElement de):m_Data(de){};
	~DBElement() = default;;
	Event<const DataElement> m_OnChanged;

	const DataElement GetData()const { return m_Data; }
	void SetData(const DataElement& data) { m_Data = data; }

private:
	DataElement m_Data;
};
