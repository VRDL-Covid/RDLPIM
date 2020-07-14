#include "rdlpch.h"
#include "subscription.hpp"


void subscription::setTimeDelay(double seconds)
{
	std::chrono::milliseconds temp { int((seconds) * 1000) };
	timeDelay = temp;
}

void subscription::setTimeLastSend()
{
	timeLastSent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

subscription::subscription()
{
	tolerance = 0;
	setTimeDelay(0);
	std::chrono::milliseconds temp{ 0 };
	timeLastSent = temp;

	lastValue = nullptr;
}


subscription::~subscription()
{
	if (lastValue != nullptr) {
		free(lastValue);
	}
}
