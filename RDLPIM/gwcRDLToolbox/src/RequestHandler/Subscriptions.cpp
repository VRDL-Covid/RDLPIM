#include"rdlpch.h"
#include"Subscriptions.h"

Subscriptions::Subscriptions()
{
	OnElementChangeCallback = CreateRef<EventCallback<const DataElement&>>();
	OnElementChangeCallback->SetCallback(BIND_EVENT_FN(Subscriptions::BuildDataBuffer));
}

bool Subscriptions::BuildDataBuffer(const DataElement& element)
{
	m_DataBuffer.append(element.Serialise());
	m_isMarked = true;
	return PROPAGATE_EVENT;
}

Subscriptions::~Subscriptions()
{
	for(std::string varName:m_SubscribedPoints){
		DataBase::GetInstance()->GetEntry(varName)->GetOnChangedEvent().unsubscribe(OnElementChangeCallback);
	}
}