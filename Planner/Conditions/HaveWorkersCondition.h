#pragma once
#include "BaseCondition.h"
class HaveWorkersCondition : public BaseCondition
{
public:
	HaveWorkersCondition(int numWorkers) {
		name = "Needs Workers";
		requiredResources[RESOURCES::STALKER] = numWorkers;
		requiredResources[RESOURCES::CARRIER] = 1;
		requiredResources[RESOURCES::DARKTEMPLAR] = 1;
	}
};