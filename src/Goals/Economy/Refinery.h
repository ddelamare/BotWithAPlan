#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Common/Strategy/Building/AssimilatorStrategy.h"
#include "Common/Util/Util.h"
class RefineryGoal : public BaseAction
{
public:
	RefineryGoal()
	{
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_REFINERY, 1));
		name = "Build Refinery";
	}

	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) override
	{
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
		auto nexs = obs->GetUnits(Unit::Alliance::Self, CompletedUnits(UNIT_TYPEID::TERRAN_COMMANDCENTER));
		if (assimilators.size() >= 1 * nexs.size() || pylons.size() == 0)
			return 0;
		else
		{
			return 1.1;
		}
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)  override
	{
		auto buildingStrategy = new AssimilatorStrategy(ABILITY_ID::BUILD_REFINERY, true, true);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_REFINERY, obs, actions, query, debug, state, buildingStrategy);
	}

};