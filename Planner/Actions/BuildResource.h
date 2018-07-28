#pragma once
#include "BaseAction.h"
#include <Common/Resource.h>
#include <Planner/Results/WorkerResult.h>
class BuildResource : public BaseAction
{
public:
	BuildResource(sc2::UNIT_TYPEID res)
	{
		results.push_back(new BaseResult(res,1));
		name = "Build " + RESOURCE_MAP[res].name;
	}

};