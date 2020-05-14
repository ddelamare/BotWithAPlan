#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Goals/Tech/Cybernetics.h"

class HighTemplarGoal : public BaseAction
{
public:
	HighTemplarGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Archives", 2, sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, 1));
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, 1));
		this->BaseAction::name = "Build High Templar";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, 2, obs);

		score = Util::FeedbackFunction(percent, .3, 2.5);
		return score;
	};

	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, obs, actions,query,debug,state);
	}
};