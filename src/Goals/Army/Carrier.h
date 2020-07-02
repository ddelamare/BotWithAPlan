#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class CarrierGoal : public BaseAction
{
public:
	CarrierGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Beacon", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_CARRIER, 1));
		this->BaseAction::name = "Build Carrier";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;

		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_CARRIER, 4, obs);

		score = Util::FeedbackFunction(percent, .3, 1.0);

		if (state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_THOR] >= 3 ||
			state->MaxEnemyUnits[UNIT_TYPEID::ZERG_MUTALISK] >= 3)
		{
			score *= 3;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(UNIT_TYPEID::PROTOSS_CARRIER, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions, query, state);
	}
};