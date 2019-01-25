#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Util.h"
#include "Common\Constants.h"
#include "Common\Strategy\Building\ExpandStrategy.h"

class ExpandGoal : public BaseAction
{
	double CLAMP = .25;
	int MIN_ARMY_PER_EXPO = 3;
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
		auto score = Util::FeedbackFunction(assignedHarvesters / (double)Constants::HARD_PROBE_CAP, .2, .5);
		auto food = obs->GetFoodArmy();
		if (food <= (MIN_ARMY_PER_EXPO * (townhalls.size() + 1))) return 0;
		if ((idealHarvesters + 20) > Constants::HARD_PROBE_CAP) return 0;
		if (townhalls.size())
		{
			int differance = (assignedHarvesters + obs->GetIdleWorkerCount()) - idealHarvesters;
			if (differance >= 2)
				score *= 1.3; // If we are near probe capacity, we need to expand
			else if (differance > -5)
				score *= 1.1; // If we are nearing probe capacity we might expand
			else
				return 0;

			if (score < CLAMP)
				return 0;
			else
				return score;
		}
		else										 
			return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto strat = new ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_NEXUS, obs, actions, query, debug, state, strat);
	}

	bool virtual HoldOtherGoals(const sc2::ObservationInterface *obs)
	{
		return holdOtherGoals && obs->GetMinerals() < 400;
	}
};