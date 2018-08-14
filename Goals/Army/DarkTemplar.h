#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Goals/Tech/Cybernetics.h>

class DarkTemplarGoal : public BaseAction, public BaseCondition
{
public:
	DarkTemplarGoal() : BaseAction(){
		this->conditions.push_back(new BaseCondition("Build Dark Shrine", 2, sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR, 1));
		this->BaseAction::name = "Build Dark Templar";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_MARAUDER] > 0)
		{
			score = 5;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto gateways = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY));
		for (auto gateway : gateways) {
			if (gateway->orders.size() == 0)
			{
				actions->UnitCommand(gateway, ABILITY_ID::TRAIN_DARKTEMPLAR);
				madeStaker = true;
			}
		}

		return madeStaker;
	}
};