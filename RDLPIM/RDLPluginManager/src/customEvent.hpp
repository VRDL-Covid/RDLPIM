#pragma once

template<typename... T>
class customEvent
{
private:
	std::vector<void(*)(T...)> callbacks;
	int noCallbacks;

public:
	int test;

	void subscribe(void(*)(T...));
	void unsubscribe(void(*)(T...));
	void raiseEvent(T... args);


	customEvent();
	~customEvent();
};

template<typename... T>
void customEvent<T...>::subscribe(void(*function)(T...))
{
	callbacks.push_back(function);
	noCallbacks++;
}

template<typename... T>
void customEvent<T...>::unsubscribe(void(*function)(T...))
{
	auto it = std::find(callbacks.begin(), callbacks.end(), function);
										 //
	callbacks.erase(it);
	noCallbacks--;
}

template<typename... T>
void customEvent<T...>::raiseEvent(T... args)
{
	auto it = callbacks.begin();

	for (it; it != callbacks.end(); it++) {
		(*it)(args...);
	}
}


template<typename... T>
customEvent<T...>::customEvent()
{

}

template<typename... T>
customEvent<T...>::~customEvent()
{

}



