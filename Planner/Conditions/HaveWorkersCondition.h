#pragma once
#include "BaseCondition.h"
class HaveWorkersCondition : public BaseCondition
{
public:
	HaveWorkersCondition(int numWorkers) {
		name = "Needs Workers";
		requiredResources[sc2::UNIT_TYPEID::PROTOSS_STALKER] = numWorkers;
		requiredResources[sc2::UNIT_TYPEID::PROTOSS_CARRIER] = 1;
		requiredResources[sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR] = 1;
	}
};