#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Util.h>
#include <Common/UnitHelpers.h>
class RoboticsGoal : public BaseAction
{
public:
	RoboticsGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->BaseAction::name = "Build Robo";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions, query, debug, state);
	}
};