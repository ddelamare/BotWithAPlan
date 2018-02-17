#pragma once
#include "BaseResult.h"

class WorkerResult : public BaseResult
{
public:
	WorkerResult(int number)
	{
		resourcesGained[sc2::UNIT_TYPEID::PROTOSS_PROBE] = number;
	}
};