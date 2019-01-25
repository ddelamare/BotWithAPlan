#pragma once
#include "BaseAction.h"
#include "Planner/Conditions/HaveWorkersCondition.h"
#include "Planner\Results\MineralResult.h"
class GatherMineralsAction : public BaseAction
{
public:
	GatherMineralsAction() : BaseAction() {
		//conditions.push_back(new HaveWorkersCondition());
		results.push_back(new MineralResult(50));
		name = "Gather Minerals";
	}

};