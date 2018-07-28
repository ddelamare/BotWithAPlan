#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Strategy/Building/BuildingStrategy.h>
#include <Common/Strategy/Building/SpiralStrategy.h>
using namespace sc2;
class GatewayGoal : public BaseAction
{
private:
	Point3D lastBuildSpot;

public:
	GatewayGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build GATEWAY", 4, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1));
		name = "Build Gateway";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto gateways = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY));
		if (!gateways.size())
		{
			return 5;
		}
		else if (gateways.size() > 7)
		{
			return 0;
		}
		else if (obs->GetFoodArmy() > 0)
		{
			return obs->GetFoodArmy() / (2 * gateways.size());
		}
		return 0;
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool success = false;

		auto buildingStrategy = new SpiralStrategy(ABILITY_ID::BUILD_GATEWAY, true,true);
		if (DistanceSquared3D(lastBuildSpot, Point3D()) == 0)
		{
			Point3D buildPos = buildingStrategy->FindPlacement(obs, actions, query, debug, state);

			auto probe = FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
			if (DistanceSquared3D(buildPos, Point3D()) > 0)
			{
				actions->UnitCommand(probe, ABILITY_ID::BUILD_GATEWAY, buildPos);
				debug->DebugSphereOut(buildPos, 3, Colors::Purple);
				lastBuildSpot = Point3D();
			}
		}

		return true;
	}

};