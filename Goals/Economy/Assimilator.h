#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"

class AssimilatorGoal : public BaseGoal
{
public:
	double virtual CalculateScore(GameState*);
	BaseAction virtual *GetAction();
};