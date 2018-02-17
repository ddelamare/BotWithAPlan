#pragma once
#include "../../Planner/Actions/BaseAction.h"

class AssimilatorGoal : public BaseAction
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs);
};