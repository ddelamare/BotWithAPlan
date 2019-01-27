#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class AdeptGoal : public BaseAction
{		   
public:
	AdeptGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ADEPT, 1));
		this->BaseAction::name = "Build Adept";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ADEPT)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent DT
		score = Util::FeedbackFunction(percent, .25, 2);
		if (state->MaxEnemyUnits[UNIT_TYPEID::ZERG_MUTALISK] >= 6
			|| state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_VOIDRAY] >= 3)
		{
			score /= 4;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_ADEPT, obs, actions, query, debug, state);
	}
};