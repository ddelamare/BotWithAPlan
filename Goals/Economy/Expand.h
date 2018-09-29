#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Util.h"
#include "Common\Strategy\Building\ExpandStrategy.h"

class ExpandGoal : public BaseAction
{
public:
	ExpandGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_NEXUS, 1));
		name = "Expand";
		holdOtherGoals = true;
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		int assignedHarvesters = 0;
		int idealHarvesters = 0;
		for (auto th : townhalls)
		{
			assignedHarvesters += th->assigned_harvesters;
			idealHarvesters += th->ideal_harvesters;
		}
		
		if (townhalls.size())
		{
			int differance = (assignedHarvesters + obs->GetIdleWorkerCount()) - idealHarvesters;
			if (differance >= 2)
				return 1.5; // If we are near probe capacity, we need to expand
			else if (differance > -5)
				return .8; // If we are nearing probe capacity we might expand
			else
				return 0;
		}
		else										 
			return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto strat = new ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_NEXUS, obs, actions, query, debug, state, strat);
	}
};