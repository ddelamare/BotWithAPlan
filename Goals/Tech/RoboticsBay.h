#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Strategy/Building/SpiralStrategy.h>

class RoboticsBayGoal : public BaseAction
{
public:
	RoboticsBayGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Robo", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, 1));
		this->BaseAction::name = "Build Robo Bay";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_ROBOTICSBAY, obs, actions, query, debug, state);
	}
};