#pragma once
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

