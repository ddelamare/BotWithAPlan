#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Strategy/Building/SpiralStrategy.h>

class TwilightCouncilGoal : public BaseAction
{
public:
	TwilightCouncilGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cyber", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->name = "Build Twilight";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool success = false;

		bool alreadyBuilding = Util().IsAnyWorkerCommitted(ABILITY_ID::BUILD_TWILIGHTCOUNCIL, obs);
		if (alreadyBuilding || !Util().HasEnoughResources(150, 100, obs)) return false;

		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		auto buildingStrategy = new SpiralStrategy(ABILITY_ID::BUILD_TWILIGHTCOUNCIL, true, true);
		Point3D buildPos = buildingStrategy->FindPlacement(obs, actions, query, debug, state);

		auto probe = Util().FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			actions->UnitCommand(probe, ABILITY_ID::BUILD_TWILIGHTCOUNCIL, buildPos);
			debug->DebugSphereOut(buildPos, 3, Colors::Purple);
		}

		return true;
	}
};