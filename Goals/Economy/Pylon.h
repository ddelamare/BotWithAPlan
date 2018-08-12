#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common\Util.h>
#include <Common\Strategy\Building\PylonStrategy.h>
using namespace sc2;
class PylonGoal : public BaseAction
{
public:
	PylonGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PYLON, 1));
		name = "Build Pylon";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 16.0f;
		int foodLeft = obs->GetFoodCap() - obs->GetFoodUsed();

		//Add food from building pylons
		auto pylonsInProgressw = obs->GetBuffData();
		auto pylonsInProgress = obs->GetUnits(Unit::Alliance::Self, UnitsInProgress(UNIT_TYPEID::PROTOSS_PYLON));
		foodLeft += pylonsInProgress.size() * 8;

		if (foodLeft > 0)
		{
			score /= foodLeft;
		}
		else if (foodLeft <= 0)
		{
			// If we are in the red, we really want that pylon.
			score = 50;
		}

		if (score < 1)
		{
			// if we have more than 16 free food, no reason to build one.
			score = 0;
		}


		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madePylon = false;
		//Is there a probe already on the way?
		bool alreadyBuilding = Util().IsAnyWorkerCommitted(ABILITY_ID::BUILD_PYLON, obs);
		if (alreadyBuilding || !Util().HasEnoughResources(100,0,obs)) return false;

		auto strat = PylonStrategy(ABILITY_ID::BUILD_PYLON,false,false);
		Point3D buildPos = strat.FindPlacement(obs, actions, query, debug, state);

		auto probe = Util().FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
		if ( DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			actions->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, buildPos);
			debug->DebugSphereOut(buildPos, 3, Colors::Teal);
		}

		return madePylon;
	}
};