#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class AdeptGoal : public BaseAction
{		   
public:
	AdeptGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 4, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ADEPT, 1));
		this->BaseAction::name = "Build Adept";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ADEPT)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent DT
		score = Util::FeedbackFunction(percent, .25, 3);
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_ADEPT, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions);
	}
};