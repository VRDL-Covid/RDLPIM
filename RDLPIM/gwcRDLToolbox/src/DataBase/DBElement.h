#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Event.h"

class DBElement
{
public:
	DBElement();
	~DBElement();


private:
	DataElement m_Data;
	Event<DataElement> m_OnChanged;
};
