#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"

class AssimilatorGoal : public BaseGoal
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs);
	BaseAction virtual *GetAction();
};