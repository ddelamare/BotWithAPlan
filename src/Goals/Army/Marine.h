#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util/UnitHelpers.h"
#include "Planner/Conditions/HaveGatewayTrainer.h"


class MarineGoal : public BaseAction
{
public:
	MarineGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Robo", 2, sc2::UNIT_TYPEID::TERRAN_BARRACKS, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_MARINE, 1));
		this->BaseAction::name = "Build Marine";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 5;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS, obs, actions, query, state);
	}
};