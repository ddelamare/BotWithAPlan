#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class TempestGoal : public BaseAction
{
public:
	TempestGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Beacon", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_TEMPEST, 1));
		this->BaseAction::name = "Build Tempest";
		//holdOtherGoals = true;
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;

		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_TEMPEST,5, obs);
		score = Util::FeedbackFunction(percent, .2, 1.0);

		if (state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_FUSIONCORE] > 0)
		{
			score *= 2;
		}

		if (state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_THOR] >= 4 
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_ARCHON] >= 4 
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_CARRIER] >= 2
			|| state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_BATTLECRUISER] >= 2
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_COLOSSUS] >= 2
			|| state->MaxEnemyUnits[UNIT_TYPEID::ZERG_ULTRALISK] >= 2
			|| state->MaxEnemyUnits[UNIT_TYPEID::ZERG_BROODLORD] >= 3
			)
		{
			score *= 2;
		}

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY] 
			+ state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON] >= 3)
		{
			score *= 3.5;
		}

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_CORRUPTOR] > 0
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER] > 3)
		{
			score /= 2.5;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_TEMPEST, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);
	}
};