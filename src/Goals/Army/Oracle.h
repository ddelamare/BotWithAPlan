#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class OracleGoal : public BaseAction
{
public:
	OracleGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Star Gate", 2, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ORACLE, 1));
		this->BaseAction::name = "Build Oracle";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto oracles = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ORACLE));
		return 0;
		double score = Util::FeedbackFunction(oracles.size(), .16, 3);
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(UNIT_TYPEID::PROTOSS_ORACLE, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions, query, state);

	}
};