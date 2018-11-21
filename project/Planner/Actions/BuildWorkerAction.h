#pragma once
#include "BaseAction.h"
#include "Results/WorkerResult.h"
class BuildWorkerAction : public BaseAction
{
public:
	BuildWorkerAction()
	{
		results.push_back(new WorkerResult(1));
		name = "Build One Worker";
	}

};