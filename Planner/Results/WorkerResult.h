#pragma once
#include "BaseResult.h"

class WorkerResult : public BaseResult
{
public:
	WorkerResult(int number)
	{
		resourcesGained[RESOURCES::PROBE] = number;
	}
};