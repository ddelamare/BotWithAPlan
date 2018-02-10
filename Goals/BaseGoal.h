#pragma once
#include "../Common/GameState.h"
#include "../Planner/Actions/BaseAction.h"
#include "sc2api\sc2_api.h"
using namespace sc2;
class BaseGoal
{
protected:
	BaseAction * goal;
	Units units;
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs) = 0;
	BaseAction virtual *GetAction() = 0;
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query) = 0;
	BaseGoal()
	{
		this->units = Units();
	}
};