#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Common/Strategy/Building/AssimilatorStrategy.h"
#include "Common/Util.h"
class AssimilatorGoal : public BaseAction
{
public:
	AssimilatorGoal()
	{
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, 1));
		name = "Build Assimilator";
	}

	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) override
	{
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		auto nexs = obs->GetUnits(Unit::Alliance::Self, CompletedUnits(UNIT_TYPEID::PROTOSS_NEXUS));
		if (assimilators.size() >= 2 * nexs.size() || pylons.size() == 0)
			return 0;
		else
		{
			return 1.1;
		}
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)  override
	{
		auto buildingStrategy = new AssimilatorStrategy(ABILITY_ID::BUILD_ASSIMILATOR, true, true);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ASSIMILATOR, UNIT_TYPEID::PROTOSS_ASSIMILATOR, obs, actions, query, debug, state, buildingStrategy);
	}

};