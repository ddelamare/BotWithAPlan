#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class ColossusGoal : public BaseAction
{
public:
	ColossusGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo STuff", 4, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_COLOSSUS, 1));
		this->BaseAction::name = "Build Collossus";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		// Counters mass marine. 
		double score = 1;
		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_COLOSSUS, 6, obs);

		score = Util::FeedbackFunction(percent, .3, 1.0);

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] > 0 
			//||state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_ROACH] >= 6
			)
		{
			score *= 2.5;
		}

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_CORRUPTOR] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_BROODLORD] > 0
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER] > 2)
		{
			score /= 2.5;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_COLOSSUS, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions);

	}
};