#pragma once
#include"DataBase/DataBase.h"

class Subscriptions
{
public:
	Subscriptions();

	~Subscriptions() = default;

	void AddVar(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_accessLock);
		m_SubscribedPoints.push_back(name); 
		DataBase::GetInstance()->GetEntry(name)->GetOnChangedEvent().subscribe(OnElementChangeCallback);
	}

	void RemVar(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_accessLock);
		for (auto it = m_SubscribedPoints.begin(); it != m_SubscribedPoints.end(); it++) {
			if (*it == name) {
				it = m_SubscribedPoints.erase(it);
				//break
				it--;
			}
		}

		DataBase::GetInstance()->GetEntry(name)->GetOnChangedEvent().unsubscribe(OnElementChangeCallback);
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
	std::mutex m_accessLock;
};