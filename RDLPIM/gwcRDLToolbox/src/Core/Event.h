#pragma once
#include "EventCallback.h"

//macos to bind generic callback functions
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) {return this->fn(std::forward<decltype(args)>(args)...);}
#define BIND_EVENT_FNO(fn,O)[this](auto&&... args) -> decltype(auto) {return O->fn(std::forward<decltype(args)>(args)...);}

const bool PROPAGATE_EVENT = false;
const bool TERMINATE_EVENT = true;

//custom even callbacks always return void
//customEvent<arg1Type,arg2Type...> myCustomEvent.

/// @brief Custom event class
/// 
/// The Event class is a template class where the arguments provided to the template form the signature of the callback.  A helper class EventCallback has been provided to hold the callback function,
/// this allows the callback to be unsubscribed as it is not possible within c++17/c++20 to determin iqeaulity between std::function(s)
/// 
/// Helper Macros have been defined to easily bind class member function from within the class (BIND_EVENT_FN) or from outside the class BIND_EVENT_FNO
/// 
/// ###Example
/// ~~~~~~~~~~~~~.cpp
/// 
/// Event<int,float,std::string> myEvent;
/// EventCallback<int,float,std::string> myCallback;
/// 
/// bool myCallbackFunction(int i, float f, std::string s){
///		std::count << "Event raised with int " << i << " float " << f << " std::string " << s << std::endl"
/// 
///		//return PROPAGATE_EVENT allow other callbacks to be executed
///     return TERMINATE_EVENT; //prevent other callbacks from being executed.
/// }
/// 
/// myCallback.SetCallback(BIND_EVENT_FN(myCallbackFunction));
/// 
/// myEvent.subscribePriority(myCallback);
/// 
/// auto id =  myEvent.subscribe([](int i, float f, std::string s){"Labmda function call back with" << i << " float " << f << " std::string " << s << std::endl"}); //an EventCallback reference is returned which can be used to unsubscribe the callback
/// 
/// myEvent.raiseEvent(1,2.0f,std::string("hello world"));
/// //only myCallback will be called
/// 
/// myEvent.unsubscribe(myCallback);
/// myEvent.raiseEvent(1,2.0f,std::string("hello world"));
/// //only the lambda will be called
/// 
/// myEvent.unsubscribe(id);
/// myEvent.raiseEvent(1,2.0f,std::string("hello world"));
/// //nothing will be called
/// 
/// // ******************** //
/// //		Macro Use		//
/// // ******************** //
/// class myClass {
///		myClass() = default;
/// 
///		bool callback(int i, float f, std::string s){
///			"Class member unction call ack with" << i << " float " << f << " std::string " << s << std::endl"
///		}
///
/// }
/// 
/// myClass exampleObj;
/// 
/// auto classCallbackID = myEvent.subscribe(BIND_EVENT_FNO(myClass::callback, exampleObj));
/// ~~~~~~~~~~~~~
/// 
/// @tparam ...T signature of the callback function, must return bool
/// 
/// @warning Event callbacks must always return bool
template<typename... T>
class Event
{
public:
	Event() = default;
	~Event() = default;

	void subscribe(const Ref<EventCallback<T...>>& callback) const;
	Ref<EventCallback<T...>> subscribe(std::function<bool(T...)> callbackFN)const;

	void subscribePriority(const Ref<EventCallback<T...>>& callback)const ;
	Ref<EventCallback<T...>> subscribePriority(std::function<bool(T...)> callbackFN)const;

	void unsubscribe(const Ref < EventCallback<T...>>& callback)const ;

	void raiseEvent(T&... mArgs) const;

	void operator = (const Event<T...>& D) = delete;


private:
	mutable std::vector<Ref<EventCallback<T...>>> callbacks;
	mutable int noCallbacks =0;
};

/// @brief Method to subscrive an EventCallback object to a method.
/// 
/// These callbacks will be called after those subscribed using subscribePriority() overloads
/// @tparam ...T Signature of the callback
/// @param callback EventCallback wrapper object
template<typename... T>
void Event<T...>::subscribe(const Ref<EventCallback<T...>>& callback)const
{
	callbacks.push_back(callback);
	noCallbacks++;
}

/// @brief A method to subscribe a std::function as a callback to the event.
/// 
/// /// These callbacks will be called after those subscribed using subscribePriority() overloads
/// @tparam ...T Signature of the callback
/// @param callbackFN a std::function which returns bool
/// @return A reference to the EventCallback object, this can be used to unsubscribe the callback.
template<typename... T>
Ref<EventCallback<T...>> Event<T...>::subscribe(std::function<bool(T...)> callbackFN) const
{
	Ref<EventCallback<T...>> callback{ new EventCallback<T...>(callbackFN) };

	callbacks.push_back(callback);
	noCallbacks++;
	return callback;
}

/// @brief A method to subscribe a priority callback
/// 
/// priorit callbacks are called before regular callbacks 
/// @tparam ...T Signature of the callback
/// @param callback EventCallback wrapper object
template<typename... T>
void Event<T...>::subscribePriority(const Ref<EventCallback<T...>>& callback)const
{
	callbacks.emplace(callbacks.begin(),callback);
	noCallbacks++;
}

/// @brief A method to subscribe a priority callback
/// 
/// priorit callbacks are called before regular callbacks
/// @tparam ...T callback signature
/// @param callbackFN std::function returning bool
/// @return A reference to the EventCallback object, this can be used to unsubscribe the callback.
template<typename... T>
Ref<EventCallback<T...>> Event<T...>::subscribePriority(std::function<bool(T...)> callbackFN) const
{
	Ref<EventCallback<T...>> callback{ new EventCallback<T...>(callbackFN) };

	callbacks.emplace(callbacks.begin(), callback);
	noCallbacks++;
	return callback;
}


/// @brief A method to unsubscribe a callback
/// 
/// This method can unsubscribe regular callbacks and priority callbacks
/// @tparam ...T The signature of the callback
/// @param callback a reference to the EventCallback object
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

/// @brief A method to raise the event
/// 
/// All subscribed callbacks will be invokde starting with priority callbacks. If a callback returns false then future callbacks will not be invoked, allowing priority callbacks to terminate the propigation of the event.
/// @tparam ...T Signature of the callbacks
/// @param ...mArgs Arguments to be passed to the callback functions.
template<typename... T>
void Event<T...>::raiseEvent(T&... mArgs) const
{
	for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
		if ((*it)->GetFunction()(std::forward<T>(mArgs)...)) {
			//terminate propagation of event
			break;
		}
				
	}
}



