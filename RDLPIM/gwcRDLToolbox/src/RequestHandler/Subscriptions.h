#pragma once
#include "DataBase/DBElement.h"

class Subscriptions
{
public:
	Subscriptions() = default;
	~Subscriptions() = default;

	void AddVar(const std::string& name) { m_SubscribedPoints.push_back(name);}

private:
	int m_a = 1;
	std::vector<std::string> m_SubscribedPoints;
};