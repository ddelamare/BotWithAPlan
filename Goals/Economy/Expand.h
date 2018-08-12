#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common\Util.h>
#include <Common\Strategy\Building\ExpandStrategy.h>

class ExpandGoal : public BaseAction
{
public:
	ExpandGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_NEXUS, 1));
		name = "Expand";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto probes = obs->GetFoodWorkers() / 22; // Times max probes per expo
		auto nexus = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		return probes / nexus.size();
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeExpandsion = false;
		//Is there a probe already on the way?
		bool alreadyBuilding = Util().IsAnyWorkerCommitted(ABILITY_ID::BUILD_NEXUS, obs);
		if (alreadyBuilding || !Util().HasEnoughResources(400,0, obs)) return false;

		auto strat = ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
		Point3D buildPos = strat.FindPlacement(obs, actions, query, debug, state);

		auto probe = Util().FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			actions->UnitCommand(probe, ABILITY_ID::BUILD_NEXUS, buildPos);
			debug->DebugSphereOut(buildPos, 3, Colors::Teal);
		}

		return madeExpandsion;
	}
};