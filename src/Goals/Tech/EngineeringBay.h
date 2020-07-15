#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class EngineeringBayGoal : public BaseAction
{
public:
	EngineeringBayGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Barracks", 2, sc2::UNIT_TYPEID::TERRAN_BARRACKS, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, 1));
		this->BaseAction::name = "Build EngineeringBay";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_ENGINEERINGBAY, obs, actions, query, debug, state);
	}
};