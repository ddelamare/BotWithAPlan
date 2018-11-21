#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class WarpPrismGoal : public BaseAction
{
public:
	WarpPrismGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo", 2, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_WARPPRISM, 1));
		this->BaseAction::name = "Build Warp Prism";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(UNIT_TYPEID::PROTOSS_WARPPRISM, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions, query);
	}
};