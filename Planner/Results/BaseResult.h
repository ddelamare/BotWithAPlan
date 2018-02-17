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
	BaseResult(sc2::UNIT_TYPEID res, int amount)
	{
		resourcesGained = ResourceMap();
		resourcesGained[res] = amount;
	}
	ResourceMap ResourcesGained();
};