#pragma once
#include "BaseAction.h"
#include "../Conditions/HaveWorkersCondition.h"
#include "../Conditions/HaveMineralsCondition.h"
class WinAction : public BaseAction
{
public:
	WinAction() : BaseAction(){
		name = "Win Action";
		conditions.push_back(new HaveWorkersCondition(5));
		//conditions.push_back(new HaveMineralsCondition(200));
	}

};