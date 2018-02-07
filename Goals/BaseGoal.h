#pragma once
#include "../Common/GameState.h"
#include "../Planner/Actions/BaseAction.h"


class BaseGoal
{
public:
	double virtual CalculateScore(GameState*) = 0;
	BaseAction virtual *GetAction() = 0;
protected:
	BaseAction * goal;
};