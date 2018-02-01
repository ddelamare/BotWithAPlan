#pragma once
#include "BaseAction.h"
#include "../Results/WorkerResult.h"
#include "../Conditions/HaveMineralsCondition.h"
class BuildMoreWorkersAction : public BaseAction
{
public:
	BuildMoreWorkersAction()
	{
		results.push_back(new WorkerResult(2));
		conditions.push_back(new HaveMineralsCondition(100));
		name = "Build More Workers";
	}

};