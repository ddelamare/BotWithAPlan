#pragma once
#include "BaseCondition.h"
class HaveMineralsCondition : public BaseCondition
{
public:
	HaveMineralsCondition(int numMinerals) {
		name = "Needs Minerals";
		requiredResources[RESOURCES::MINERALS] = numMinerals;
	}
};