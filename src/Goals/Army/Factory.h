#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/BuildingStrategy.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
using namespace sc2;
class FactoryGoal : public BaseAction
{
public:
	FactoryGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Barracks", 4, sc2::UNIT_TYPEID::TERRAN_BARRACKS, 1, sc2::UNIT_TYPEID::TERRAN_SCV, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_FACTORY, 1));
		name = "Build Factory";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_FACTORY, obs, actions, query, debug, state);
	}
};