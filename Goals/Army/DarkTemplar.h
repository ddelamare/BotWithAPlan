#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Goals/Tech/Cybernetics.h>

class DarkTemplarGoal : public BaseAction
{
public:
	DarkTemplarGoal() : BaseAction(){
		this->conditions.push_back(new BaseCondition("Build Dark Shrine", 2, sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR, 1));
		this->BaseAction::name = "Build Dark Templar";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] > 0)
		{
			score = 2;
		}
		return score;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_DARKTEMPLAR, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions);

	}
};