#pragma once
#include"Core/buffer.hpp"
#include"reqElement.hpp"
#include"subscription.hpp"

enum sourceTypes { External, RDL_Process};

class dataElement
{
private:
	

public:
	buffer varName;
	responceElement data;
	sourceTypes source;
	std::vector<subscription> subscriptions;

	int findSubscription(int uID);
	void rdlUpdate();

	void markSubscriptionsForSending();

	dataElement();
	dataElement(buffer iName);
	~dataElement();

};

