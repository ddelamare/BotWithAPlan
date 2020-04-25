#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/BuildingStrategy.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
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
		auto warpgates = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
		unsigned int totalGates = gateways.size() + warpgates.size();

		if (!totalGates)
		{
			return 5;
		}

		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		int assignedHarvesters = 0;
		for (auto th : townhalls)
		{
			assignedHarvesters += th->assigned_harvesters;
		}
		auto gas = obs->GetUnits(sc2::Unit::Alliance::Self, IsGasBuilding());
		for (auto th : gas)
		{
			assignedHarvesters += th->assigned_harvesters;
		}
		// Ideally should have 3 gates per fully mining base
		auto score =  assignedHarvesters / ((1 + totalGates) * (22.0));
		double CLAMP = .35;
		if (score < CLAMP)
			return 0;

		return score;

	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_GATEWAY, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions, query, debug, state);
	}

};