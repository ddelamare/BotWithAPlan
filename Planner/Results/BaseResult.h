#pragma once
#include <map>
#include "../../Common/Resource.h"
using namespace std;

class BaseResult // This is abstract
{
protected:
	ResourceMap resourcesGained;
public:
	BaseResult() {
		resourcesGained = ResourceMap();
	}
	BaseResult(RESOURCES res, int amount)
	{
		resourcesGained = ResourceMap();
		resourcesGained[res] = amount;
	}
	ResourceMap ResourcesGained();
};