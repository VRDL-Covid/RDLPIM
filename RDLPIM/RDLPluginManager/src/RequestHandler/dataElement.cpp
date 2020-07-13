#include "rdlpch.h"
#include "dataElement.hpp"


dataElement::dataElement()
{
}

dataElement::dataElement(buffer iName)
{
	
	varName = iName;
	rdlData tempData(varName);

	if (!strncmp("ERR", tempData.ctype, 3)) {
		source = External;
		data.varName = varName;
	}
	else {
		source = RDL_Process;
		data.set(tempData);
	}
}

dataElement::~dataElement()
{

}

int dataElement::findSubscription(int uID)
{
	std::vector<subscription>::iterator it = subscriptions.begin();

	for (it; it != subscriptions.end(); it++) {

		if (it->userID == uID) {
			return std::distance(subscriptions.begin(), it);
		}
	}

	return -1;
}

void dataElement::rdlUpdate()
{
	if (source != RDL_Process) {
		return;
	}
	rdlData liveData(varName);
	data.set(liveData);
}

void dataElement::markSubscriptionsForSending()
{
	std::vector<subscription>::iterator subIt = subscriptions.begin();

	std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::milliseconds delta;

	for (subIt; subIt != subscriptions.end(); subIt++) {
		//check sufficient amount of time has passed
		delta = now - subIt->timeLastSent;
		if (delta >= subIt->timeDelay) {
			//if data has changed
			if (strcmp(subIt->lastValue, data.data)) {
				//if double has tolerance been breached
				if (!strncmp("double", data.type.contents, 6)) {
					double diff = abs(*(double*)subIt->lastValue - *(double*)data.data);
					if (diff >= subIt->tolerance) {
						subIt->markedForSending = true;
					}
				}
				else {
					subIt->markedForSending = true;
				}
			}
		}	
	}


}