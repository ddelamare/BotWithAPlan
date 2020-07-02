#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Analyzers/RushAnalyzer.h"
#include "Common\Util.h"
#include "Common\Constants.h"
#include "Common\Strategy\Building\ExpandStrategy.h"

class ExpandGoal : public BaseAction
{
	double CLAMP = .25;
	int MIN_ARMY_PER_EXPO = 0;
	// Roughly this value means that it gets more greedy with time
	float GREED_DAMPER = 1500.0;
	RushAnalyzer rushAnalyzer;

public:
	ExpandGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_NEXUS, 1));
		name = "Expand";
		holdOtherGoals = true;
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		auto townHallsBuilding = obs->GetUnits(sc2::Unit::Alliance::Self, UnitsInProgress(UNIT_TYPEID::PROTOSS_NEXUS));

		if (townHallsBuilding.size()) return 0;

		// If there's a rush happening we should delay
		auto rushChance = rushAnalyzer.GetRushPossibiliy(obs);
		if (rushChance > 1)
		{
			return 0;
		}

		int assignedHarvesters = 0;
		int idealHarvesters = 0;
		for (auto th : townhalls)
		{
			assignedHarvesters += th->assigned_harvesters;
			idealHarvesters += th->ideal_harvesters;
		}
		auto gas = obs->GetUnits(sc2::Unit::Alliance::Self, IsGasBuilding());
		for (auto th : gas)
		{
			assignedHarvesters += th->assigned_harvesters;
			idealHarvesters += th->ideal_harvesters;
		}

		auto score = Util::FeedbackFunction(assignedHarvesters / (double)Constants::HARD_WORKER_CAP, .25, .5);
		auto food = obs->GetFoodArmy();
		if (food <= (MIN_ARMY_PER_EXPO * (townhalls.size() + 1))) return 0;
		if (assignedHarvesters < 8) return 0;
		if (idealHarvesters > Constants::HARD_WORKER_CAP) return 0;
		if (townhalls.size())
		{
			int differance = (assignedHarvesters + obs->GetIdleWorkerCount()) - idealHarvesters;
			if (differance >= 2)
				score *= 1.3; // If we are near probe capacity, we need to expand
			else if (differance > -5)
				score *= 1.1; // If we are nearing probe capacity we might expand
			else if (townhalls.size() > 2)
				return 0;

			if (townhalls.size() == 1)
			{
				score = 1;
			}

			if (townhalls.size() < 2)
			{
				score *= 3.5;
			}

			score *= obs->GetGameLoop() / (GREED_DAMPER * (townhalls.size()));
			if (score < CLAMP && townhalls.size() > 1) // If we are losing a lot of units, don't expand
				return 0;
			else
				// If we are ahead, we can expand more.  
				return score;
		}
		else
			return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto strat = new ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_NEXUS, obs, actions, query, debug, state, strat);
	}

	bool virtual HoldOtherGoals(const sc2::ObservationInterface* obs)
	{
		return holdOtherGoals && obs->GetMinerals() < 400;
	}
};