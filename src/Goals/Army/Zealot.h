#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/UnitHelpers.h"
#include "Planner/Conditions/HaveGatewayTrainer.h"


class ZealotGoal : public BaseAction
{
public:
	ZealotGoal() : BaseAction() {
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, 1));
		this->BaseAction::name = "Build Zealot";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;
		auto percent = Util::GetUnitPercent(IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT), 2, obs);
		score = Util::FeedbackFunction(percent, .35, 3);

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0)
		{
			score *= 4;
		}

		if (obs->GetMinerals() > 1000 && obs->GetVespene() < 200)
		{
			score *= 2;
		}

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_ROACH] > 5
			)
		{
			score /= 3;
		}

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_REAPER] >= 4)
		{
			score /= 4;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_ZEALOT, obs, actions, query, debug, state);
	}
};