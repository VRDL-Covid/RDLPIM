#pragma once
#include<stdio.h>
#include<iostream>
#include<chrono>
class subscription
{

	
public:
	int userID;
	double tolerance;
	char* lastValue;
	bool markedForSending;

	std::chrono::milliseconds timeDelay;
	std::chrono::milliseconds timeLastSent;

	void setTimeDelay(double seconds);
	void setTimeLastSend();
	subscription();
	~subscription();
};

