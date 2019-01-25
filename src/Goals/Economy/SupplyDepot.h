#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Util.h"
#include "Common\Strategy\Building\PylonStrategy.h"
#include "Common\UnitHelpers.h"

using namespace sc2;
class SupplyDepotGoal : public BaseAction
{
public:
	SupplyDepotGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, 1));
		name = "Build Supply Depot";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 20.0f;
		size_t foodLeft = obs->GetFoodCap() - obs->GetFoodUsed();

		//Add food from building SupplyDepots
		auto SupplyDepotsInProgress = obs->GetUnits(Unit::Alliance::Self, UnitsInProgress(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
		foodLeft += SupplyDepotsInProgress.size() * 8;
		if (Util::IsAnyWorkerCommitted(ABILITY_ID::BUILD_SUPPLYDEPOT, obs))
		{
			foodLeft += 8;
		}

		if (foodLeft > 0)
		{
			score /= foodLeft;
		}
		else if (foodLeft <= 0)
		{
			// If we are in the red, we really want that SupplyDepot.
			score = 50;
		}

		if (score < 1 || obs->GetFoodCap() == 200)
		{
			// if we have more than 16 free food, no reason to build one.
			score = 0;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		auto buildingStrat = new PylonStrategy(ABILITY_ID::BUILD_SUPPLYDEPOT,false,false, (Race)GetAgentRace());
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SUPPLYDEPOT, obs, actions, query, debug, state, buildingStrat);
	}
};