#pragma once
#include"DataBase/DataBase.h"

class Subscriptions
{
public:
	Subscriptions();

	~Subscriptions() = default;

	void AddVar(const std::string& name)
	{
		m_SubscribedPoints.push_back(name); DataBase::GetInstance()->GetEntry(name)->GetOnChangedEvent().subscribe(OnElementChangeCallback);
	}

	Buffer GetOutData() { return m_DataBuffer;}

	void Clear() { m_DataBuffer.set(""); m_isMarked = false; }
	
	bool IsMarkerForSending() { return m_isMarked; }
public://callbacks
	bool BuildDataBuffer(const DataElement& element);
	Ref<EventCallback<const DataElement&>> OnElementChangeCallback;
private:
	int m_a = 1;
	std::vector<std::string> m_SubscribedPoints;
	Buffer m_DataBuffer;
	bool m_isMarked = false;
};