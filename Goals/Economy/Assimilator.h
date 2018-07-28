#pragma once
#include "../../Planner/Actions/BaseAction.h"

class AssimilatorGoal : public BaseAction
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) override
	{
		return 0;
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)  override
	{
		return false;
	}

};