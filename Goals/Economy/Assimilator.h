#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Common/Strategy/Building/AssimilatorStrategy.h>
#include <Common/Util.h>
class AssimilatorGoal : public BaseAction
{
public:
	AssimilatorGoal()
	{
		name = "Build Assimilator";
	}

	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) override
	{
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
		auto nexs = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
		if (assimilators.size() >= 2 * nexs.size())
			return 0;
		else
		{
			return 1;
		}
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)  override
	{
		bool builtAssim = false;

		bool alreadyBuilding = Util().IsAnyWorkerCommitted(ABILITY_ID::BUILD_ASSIMILATOR, obs);
		if (alreadyBuilding) return false;

		auto buildingStrategy = new AssimilatorStrategy(ABILITY_ID::BUILD_ASSIMILATOR, true, true);
		Point3D buildPos = buildingStrategy->FindPlacement(obs, actions, query, debug, state);
		
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			auto probe = Util().FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
			actions->UnitCommand(probe, ABILITY_ID::BUILD_ASSIMILATOR, buildingStrategy->foundUnit);
			debug->DebugSphereOut(buildPos, 3, Colors::Purple);
			builtAssim = true;
		}

		return builtAssim;
	}

};