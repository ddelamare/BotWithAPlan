#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class VoidRayGoal : public BaseAction
{
public:
	VoidRayGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Star Gate", 2, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_VOIDRAY, 1));
		this->BaseAction::name = "Build VoidRay";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score;

		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_VOIDRAY, 4, obs);

		score = Util::FeedbackFunction(percent, .2, 1);

		//if (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SCV] > 0)
		//{
		//	score *= 2.1;
		//}


		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_ULTRALISK] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 4
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER] > 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_LIBERATOR] > 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_LIBERATORAG] > 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_ARCHON] > 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_BROODLORD] > 2
			|| state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 2)
		{
			score *= 2.5;
		}

		// If reaper rush, tech to air
		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_REAPER] > 2
			&& state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARINE] < 2)
		{
			score *= 3;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_VOIDRAY, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);
	}
};