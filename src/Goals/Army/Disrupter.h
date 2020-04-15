#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class DisruptorGoal : public BaseAction
{
public:
	DisruptorGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo STuff", 4, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_DISRUPTOR, 1));
		this->BaseAction::name = "Build Disruptor";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		// Counters mass marine. 
		double score = 1;
		int unitFood = 3 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTOR)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots
		score = Util::FeedbackFunction(percent, .2, 2.0);

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] > 0 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARAUDER] >= 6
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARINE] >= 6)
		{
			score *= 1.5;
		}

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] >= 3
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] >= 3)
		{
			score *= 1.5;
		}

		//// Disruptors can easily kill cannons even with shield batteries
		//if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY] >= 2
		//	|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON] >= 3)
		//{
		//	score *= 3.5;
		//}

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_BROODLORD] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::PROTOSS_CARRIER] > 0
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_MUTALISK] >= 5)
		{
			score /= 2;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_DISRUPTOR, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions);

	}
};