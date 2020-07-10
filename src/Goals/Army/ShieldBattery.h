#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
#include "Common/Util/Util.h"
#include "Common/Analyzers/RushAnalyzer.h"

class ShieldBatteryGoal : public BaseAction
{
	RushAnalyzer rushAnalyzer;

public:
	ShieldBatteryGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build pylon", 4, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY, 1));
		this->BaseAction::name = "Build Shield Battery";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto score = 0.0;

		int batteries = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)).size();
		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, CompletedUnits(IsTownHall())).size();

		if (townhalls <= batteries || townhalls <= 1)
		{
			return 0;
		}

		auto rushChance = rushAnalyzer.GetRushPossibiliy(obs);
		if (rushChance > 1)
		{
			return 2;
		}

		return 1;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto start = new SpiralStrategy(ABILITY_ID::BUILD_SHIELDBATTERY, false, true, state->selfRace, true);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_SHIELDBATTERY, UNIT_TYPEID::PROTOSS_SHIELDBATTERY, obs, actions, query, debug, state);
	}
};