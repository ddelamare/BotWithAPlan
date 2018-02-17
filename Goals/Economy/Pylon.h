#pragma once
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"
using namespace sc2;
class PylonGoal : public BaseAction
{
public:
	PylonGoal() : BaseAction() {
		name = "Build Pylon";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
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
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug)
	{
		bool madePylon = false;
		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		auto nexii = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
		bool works = query->Placement(ABILITY_ID::BUILD_PYLON, nexii[0]->pos + Point2D(0, -5));
		auto probe = obs->GetUnits(Unit::Alliance::Self, IsWorker())[0];
		actions->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, nexii[0]->pos + Point2D(0, -5));
		debug->DebugSphereOut(nexii[0]->pos + Point3D(0, -5,0) + Point3D(0, 0, 1), 3, Colors::Teal);
		//TODO: Find location  away from minerals
		//TODO: Find Nearby Probe
		//TODO: Build pylon

		return madePylon;
	}
};