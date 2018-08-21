#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>

class ImmortalGoal : public BaseAction
{
public:
	ImmortalGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo", 2, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_IMMORTAL, 1));
		this->BaseAction::name = "Build Immortal";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_IMMORTAL, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions);
	}
};