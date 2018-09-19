#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class StarGateGoal : public BaseAction
{
public:
	StarGateGoal() : BaseAction() 
	{
		this->conditions.push_back(new BaseCondition("Build Cyber", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
		this->BaseAction::name = "Build StarGate";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state)  {
		auto stargates = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_STARGATE));
		auto nexus = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		return (nexus.size() + 1) / (stargates.size() + 2);
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_STARGATE, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions, query, debug, state);
	}
};