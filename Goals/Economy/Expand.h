#pragma once
#include "../../Planner/Actions/BaseAction.h"

class ExpandGoal : public BaseAction
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs) = 0;

};