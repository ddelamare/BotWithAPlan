#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class MothershipGoal : public BaseAction
{
public:
	MothershipGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Beacon", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIP, 1));
		this->BaseAction::name = "Build Mothership";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		int unitFood =  obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_MOTHERSHIP)).size();
		if (unitFood > 0) return 0;

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(UNIT_TYPEID::PROTOSS_MOTHERSHIP, UNIT_TYPEID::PROTOSS_NEXUS, obs, actions, state);
	}
};