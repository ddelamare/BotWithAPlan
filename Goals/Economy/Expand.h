#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"

class ExpandGoal : public BaseGoal
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs) = 0;
	BaseAction virtual *GetAction();
};