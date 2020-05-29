#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Goals/Tech/Cybernetics.h"

class DarkTemplarGoal : public BaseAction
{
public:
	DarkTemplarGoal() : BaseAction(){
		this->conditions.push_back(new BaseCondition("Build Dark Shrine", 2, sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, 1));
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR, 1));
		this->BaseAction::name = "Build Dark Templar";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		//if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] > 0)
		//{
		//	int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)).size();
		//	auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots
		//	score = Util::FeedbackFunction(percent, .2, 2.0);
		//}
		return score;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_DARKTEMPLAR, obs, actions, query, debug, state);
	}
};