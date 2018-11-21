#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class ObserverGoal : public BaseAction
{
public:
	ObserverGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo", 2, sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_OBSERVER, 1));
		this->BaseAction::name = "Build Observer";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_OBSERVER)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); 

		if (state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_LURKERMP] > 0 
			|| state->ObservedUnits[sc2::UNIT_TYPEID::ZERG_SWARMHOSTMP] > 0 ||
			state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_BANSHEE] > 0
			|| state->HasCloakedUnits)
		{
			score = Util::FeedbackFunction(percent, .05, 3);
		}

		if (percent >= 1 && score < .1)	 // We really don;t need that many
			return 0;
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_OBSERVER, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, obs, actions);
	}
};