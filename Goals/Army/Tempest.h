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
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;
		int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_TEMPEST)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots
		score = Util::FeedbackFunction(percent, .3, 1.0);

		if (state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_THOR] > 4 
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_ARCHON] > 4 || state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_CARRIER] > 2
			|| state->MaxEnemyUnits[UNIT_TYPEID::ZERG_ULTRALISK] > 4)
		{
			score *= 2;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_TEMPEST, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);

	}
};