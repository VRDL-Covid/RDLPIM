#pragma once
#include "EventCallback.h"

const bool PROPAGATE_EVENT = false;
const bool TERMINATE_EVENT = true;

//custom even callbacks always return void
//customEvent<arg1Type,arg2Type...> myCustomEvent.
template<typename... T>
class Event
{
public:
	Event() = default;
	~Event() = default;

	uint32_t subscribe(const Ref<EventCallback<T...>>& callback) const;
	uint32_t subscribe(std::function<bool(T...)> callbackFN)const;

	uint32_t subscribePriority(const Ref<EventCallback<T...>>& callback)const ;
	uint32_t subscribePriority(std::function<bool(T...)> callbackFN)const;

	void unsubscribe(const Ref < EventCallback<T...>>& callback)const ;
	void unsubscribe(const uint32_t& id)const;

	void raiseEvent(T&... mArgs);

	void operator = (const Event<T...>& D) = delete;


private:
	mutable std::vector<Ref<EventCallback<T...>>> callbacks;
	mutable int noCallbacks =0;
};

template<typename... T>
uint32_t Event<T...>::subscribe(const Ref<EventCallback<T...>>& callback)const
{
	callbacks.push_back(callback);
	noCallbacks++;
	return callback->GetID();
}

template<typename... T>
uint32_t Event<T...>::subscribe(std::function<bool(T...)> callbackFN) const
{
	Ref<EventCallback<T...>> callback{ new EventCallback<T...>(callbackFN) };

	callbacks.push_back(callback);
	noCallbacks++;
	return callback->GetID();
}

template<typename... T>
uint32_t Event<T...>::subscribePriority(const Ref<EventCallback<T...>>& callback)const
{
	callbacks.emplace(callbacks.begin(),callback);
	noCallbacks++;
	return callback->GetID();
}

template<typename... T>
uint32_t Event<T...>::subscribePriority(std::function<bool(T...)> callbackFN) const
{
	Ref<EventCallback<T...>> callback{ new EventCallback<T...>(callbackFN) };

	callbacks.emplace(callbacks.begin(), callback);
	noCallbacks++;
	return callback->GetID();
}

template<typename... T>
void Event<T...>::unsubscribe(const Ref<EventCallback<T...>>& callback) const
{
	for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
		if ((**it).GetID() == (*callback).GetID()){
			callbacks.erase(it);
			break;
		}
	}
}

template<typename... T>
void Event<T...>::unsubscribe(const uint32_t& id) const
{
	for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
		if ((**it).GetID() == id) {
			callbacks.erase(it);
			break;
		}
	}
}

template<typename... T>
void Event<T...>::raiseEvent(T&... mArgs)
{
	for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
		if ((*it)->GetFunction()(std::forward<T>(mArgs)...)) {
			//terminate propagation of event
			break;
		}
				
	}
}



