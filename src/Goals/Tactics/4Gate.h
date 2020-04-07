#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"

class Do4GateGoal : public BaseAction
{
public:
	Do4GateGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Cheese", 4, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 4, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1));
		this->BaseAction::name = "4Gate";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return true;
	}
};