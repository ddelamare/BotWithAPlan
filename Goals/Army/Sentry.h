#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "Planner/Conditions/HaveGatewayTrainer.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class SentryGoal : public BaseAction
{
public:
	SentryGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_SENTRY, 1));
		this->BaseAction::name = "Build Sentry";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_MUTALISK] >= 5
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] >= 5
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_VOIDRAY] >= 5
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::PROTOSS_CARRIER] >= 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_ROACH] >= 5
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARINE] >= 10)
		{
			int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_SENTRY)).size();
			auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots

			score = Util::FeedbackFunction(percent, .05, 1.5);
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_SENTRY, obs, actions, query, debug, state);
	}
};																    