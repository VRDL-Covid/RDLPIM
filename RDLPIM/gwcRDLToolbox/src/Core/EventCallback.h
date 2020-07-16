#pragma once
#include "rdlpch.h"
#include "Core.h"
//todo put somewhere else
#define BIND_EVENT_FN0(x) std::bind(&x, this)
#define BIND_EVENT_FN1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_EVENT_FN2(x) std::bind(&x, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_EVENT_FN3(x) std::bind(&x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_EVENT_FN4(x) std::bind(&x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND_EVENT_FN5(x) std::bind(&x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
#define BIND_EVENT_FN6(x) std::bind(&x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)

#define BIND_EVENT_FNO0(x,y) std::bind(&x, &y)
#define BIND_EVENT_FNO1(x,y) std::bind(&x, &y, std::placeholders::_1)
#define BIND_EVENT_FNO2(x,y) std::bind(&x, &y, std::placeholders::_1, std::placeholders::_2)
#define BIND_EVENT_FNO3(x,y) std::bind(&x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_EVENT_FNO4(x,y) std::bind(&x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND_EVENT_FNO5(x,y) std::bind(&x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
#define BIND_EVENT_FNO6(x,y) std::bind(&x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)


#define BIND_EVENT_GFN(x) std::bind(x, this, std::placeholders::_1)

#define BIND_EVENT_GFN0(x) std::bind(x, this)
#define BIND_EVENT_GFN1(x) std::bind(x, this, std::placeholders::_1)
#define BIND_EVENT_GFN2(x) std::bind(x, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_EVENT_GFN3(x) std::bind(x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_EVENT_GFN4(x) std::bind(x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND_EVENT_GFN5(x) std::bind(x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
#define BIND_EVENT_GFN6(x) std::bind(x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)

#define BIND_EVENT_GFNO0(x,y) std::bind(x, &y)
#define BIND_EVENT_GFNO1(x,y) std::bind(x, &y, std::placeholders::_1)
#define BIND_EVENT_GFNO2(x,y) std::bind(x, &y, std::placeholders::_1, std::placeholders::_2)
#define BIND_EVENT_GFNO3(x,y) std::bind(x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_EVENT_GFNO4(x,y) std::bind(x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define BIND_EVENT_GFNO5(x,y) std::bind(x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)
#define BIND_EVENT_GFNO6(x,y) std::bind(x, &y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)



class IEventCallback
{
public:
	static uint32_t GetNextID(){ return s_NextID++; }
	virtual ~IEventCallback() = default;
private:
	IEventCallback() = default;
	static uint32_t s_NextID;
		
};


template<typename ...T>
class EventCallback
{
public:
	EventCallback()
	{
		m_ID = IEventCallback::GetNextID();
	}

	void SetCallback(std::function<bool(T...)> callbackFunction)
	{
		m_Callback = callbackFunction;
	}

	EventCallback(std::function<bool(T...)> callbackFunction)
	{
		m_ID = IEventCallback::GetNextID();
		m_Callback = callbackFunction;
	}

	virtual ~EventCallback() = default;

	uint32_t GetID() { return m_ID; }
	std::function<bool(T...)>& GetFunction() { return m_Callback; }

private:
		
	uint32_t m_ID;
	std::function<bool(T...)> m_Callback;
};
