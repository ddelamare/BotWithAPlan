#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class ImmortalGoal : public BaseAction
{
public:
	ImmortalGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo", 2, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_IMMORTAL, 1));
		this->BaseAction::name = "Build Immortal";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;
		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_IMMORTAL, 4, obs);
		score = Util::FeedbackFunction(percent, .6, 4);

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_ROACH] > 5)
		{
			score *= 2.0;
		}

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_ULTRALISK] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0
			)
		{
			score *= 1.5;
		}
		if (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_THOR] > 0)
		{
			score *= 1.2;
		}
		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_STALKER] > 8 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON] > 0
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_SPINECRAWLER] > 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_BUNKER] > 1)
		{
			score *= 1.4;
		}

		if (state->ObservedUnits[sc2::UNIT_TYPEID::PROTOSS_VOIDRAY] > 1
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER] > 1
			|| state->ObservedUnits[sc2::UNIT_TYPEID::PROTOSS_CARRIER] > 1
			|| state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_BROODLORD] > 1
			)
		{
			score /= 1.2;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_IMMORTAL, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions);
	}
};