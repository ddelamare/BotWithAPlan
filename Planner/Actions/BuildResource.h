#pragma once
#include "BaseAction.h"
#include "../../Common/Resource.h"
#include "../Results/WorkerResult.h"
class BuildResouce : public BaseAction
{
public:
	BuildResouce()
	{
		conditions.push_back(new HaveMineralsCondition(50));
		results.push_back(new WorkerResult(1));
		name = "Build " + RESOURCES_MAP[res].name;
	}

};