#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/UnitHelpers.h>

class ZealotGoal : public BaseAction
{
public:
	ZealotGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Gateway", 2, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, 1));
		this->BaseAction::name = "Build Zealot";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;
		if (obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT)).size() > 5) return 0;

		if (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 || state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0)
		{
			score = 3;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_ZEALOT, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions);
	}
};