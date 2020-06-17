#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util.h"
#include "Common/UnitHelpers.h"
class RoboticsGoal : public BaseAction
{
public:
	RoboticsGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->BaseAction::name = "Build Robo";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		auto robos = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY));
		if (!robos.size()) return 0; // Don;t build it unless we need it
		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, CompletedUnits(IsTownHall()));
		int assignedHarvesters = 0;
		for (auto th : townhalls)
		{
			assignedHarvesters += th->assigned_harvesters;
		}
		auto gas = obs->GetUnits(sc2::Unit::Alliance::Self, IsGasBuilding());
		for (auto th : gas)
		{
			assignedHarvesters += th->assigned_harvesters;
		}
		// Ideally should have (half) a  robo per fully mining  base
		auto score = assignedHarvesters / ((robos.size() * 4) * (22.0));

		double CLAMP = .45;
		if (score < CLAMP)
			return 0;

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions, query, debug, state);
	}
};