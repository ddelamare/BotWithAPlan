#pragma once
#include "BaseAction.h"
#include "../../Common/Resource.h"
#include "../Results/WorkerResult.h"
class BuildResource : public BaseAction
{
public:
	BuildResource(RESOURCES res)
	{
		results.push_back(new BaseResult(res,1));
		name = "Build " + RESOURCE_MAP[res].name;
	}

};