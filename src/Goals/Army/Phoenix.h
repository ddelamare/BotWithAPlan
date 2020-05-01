#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class PhoenixGoal : public BaseAction
{
public:
	PhoenixGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Star Gate", 2, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PHOENIX, 1));
		this->BaseAction::name = "Build Phoenix";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_PHOENIX, 2, obs);

		if (state->MaxEnemyUnits[UNIT_TYPEID::ZERG_MUTALISK] >= 6 
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_VOIDRAY] >= 3
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_BANSHEE] > 2)
		{
			score = Util::FeedbackFunction(percent, .4, 2);
		}
		else
		{
			score *= Util::FeedbackFunction(percent, .2, 1);
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_PHOENIX, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);

	}
};