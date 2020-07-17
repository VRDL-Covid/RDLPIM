#include"rdlpch.h"
#include"Subscriptions.h"

Subscriptions::Subscriptions()
{
	OnElementChangeCallback = CreateRef<EventCallback<const DataElement&>>();
	OnElementChangeCallback->SetCallback(BIND_EVENT_FN1(Subscriptions::BuildDataBuffer));
}

bool Subscriptions::BuildDataBuffer(const DataElement& element)
{
	m_DataBuffer.append(element.Serialise());
	m_isMarked = true;
	return PROPAGATE_EVENT;
}