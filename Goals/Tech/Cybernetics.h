#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Strategy/Building/SpiralStrategy.h>

class CyberneticsGoal : public BaseAction, public BaseCondition
{
public:
	CyberneticsGoal() : BaseAction() , BaseCondition("Build CYBERNETICS", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1) {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->BaseAction::name = "Build Cybernetics";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		double score = 0;
		return score + 1;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool success = false;

		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		auto buildingStrategy = new SpiralStrategy(ABILITY_ID::BUILD_CYBERNETICSCORE, true, true);
		Point3D buildPos = buildingStrategy->FindPlacement(obs, actions, query, debug, state);

		auto probe = FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			actions->UnitCommand(probe, ABILITY_ID::BUILD_CYBERNETICSCORE, buildPos);
			debug->DebugSphereOut(buildPos, 3, Colors::Purple);
		}

		return true;
	}
};