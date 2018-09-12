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
		double score = 1;
		int unitFood = 4 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_VOIDRAY)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots
		score = Util::FeedbackFunction(percent, .2, 1.5);

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_ULTRALISK] > 0 || state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 || state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0)
		{
			score *= 1.5;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_VOIDRAY, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);

	}
};