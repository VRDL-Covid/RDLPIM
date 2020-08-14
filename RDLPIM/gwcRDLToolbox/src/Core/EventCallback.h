#pragma once
#include "rdlpch.h"
#include "Core.h"
//todo put somewhere else


/// @brief A class to aid with getting the next available callback id
/// 
/// These Ids are ultimately used to identify a callback should it need to be unsubscribed
class IEventCallback
{
public:
	static uint32_t GetNextID(){ return s_NextID++; }
	virtual ~IEventCallback() = default;
private:
	IEventCallback() = default;
	static uint32_t s_NextID;
		
};


/// @brief The EventCallback class is a utility class to the Event system. It is used to wrap a function which can then be traced.
/// 
/// EventCallback referneces can be maintained and used to subscribe or unsubscribe callback functions to an event.
/// @tparam ...T Signature of the bound callback function.
template<typename ...T>
class EventCallback
{
public:
	EventCallback()
	{
		m_ID = IEventCallback::GetNextID();
	}

	/// @brief A method to bind a function to the EventCallback object
	/// @param callbackFunction std::function returning bool
	/// @attention See Event documentation and how the helper macros BIND_EVENT_FN and BIND_EVENT_FNO can be used to help bind functions
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

	/// @brief A method to get the ID of the callback wrapper object
	/// @return the ID
	const uint32_t GetID() const { return m_ID; }
	
	/// @brief A method to return the function wrapped by the EventCallback object
	/// @return std::function returning bool and varidiac arguments
	std::function<bool(T...)>& GetFunction() { return m_Callback; }

private:
	uint32_t m_ID;
	std::function<bool(T...)> m_Callback;
};
