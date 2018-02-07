#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"

class ChronoGoal : public BaseGoal
{
public:
	double virtual CalculateScore(GameState*) = 0;
	BaseAction virtual *GetAction();
};