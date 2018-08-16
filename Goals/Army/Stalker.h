#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>

class StalkerGoal : public BaseAction
{
public:
	StalkerGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 4, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STALKER, 1));
		this->BaseAction::name = "Build Stalker";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		return score + 2;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_STALKER, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions);
	}
};