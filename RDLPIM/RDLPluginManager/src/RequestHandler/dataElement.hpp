#pragma once
#include"buffer.hpp"
#include"reqElement.hpp"
#include"subscription.hpp"
#include<vector>
#include<chrono>


enum sourceTypes { External, RDL_Process};

class dataElement
{
private:
	

public:
	buffer varName;
	reqElement data;
	sourceTypes source;
	std::vector<subscription> subscriptions;

	int findSubscription(int uID);
	void rdlUpdate();

	void markSubscriptionsForSending();

	dataElement();
	dataElement(buffer iName);
	~dataElement();

};
