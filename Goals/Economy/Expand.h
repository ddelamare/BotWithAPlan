#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common\Util.h>
#include <Common\Strategy\Building\ExpandStrategy.h>

class ExpandGoal : public BaseAction
{
public:
	ExpandGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_NEXUS, 1));
		name = "Expand";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto probes = obs->GetFoodWorkers() / 22; // Times max probes per expo
		auto nexus = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		return probes / nexus.size();
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto strat = new ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_NEXUS, obs, actions, query, debug, state, strat);
	}
};