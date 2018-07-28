#pragma once
#include "BaseAction.h"
#include <Planner/Conditions/HaveWorkersCondition.h>
class WinAction : public BaseAction
{
public:
	WinAction() : BaseAction(){
		name = "Win Action";
		conditions.push_back(new HaveWorkersCondition(5));
	}

};