#pragma once
#include "Common/Util.h"
using namespace sc2;

class PhoenixLift
{
	UnitTypeData unitInfo;
	UnitTypes* allUnitInfo;
public:
	PhoenixLift(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
PhoenixLift::PhoenixLift(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	allUnitInfo = &state->UnitInfo;
	unitInfo = (*allUnitInfo)[(int)UNIT_TYPEID::PROTOSS_PHOENIX];
}
void PhoenixLift::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto phoenixes = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PHOENIX));
	for (auto ph : phoenixes)
	{
		auto enemyFlyersUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsFlying(), ph->pos, obs, unitInfo.sight_range);
		if (enemyFlyersUnits.size()) return;
		auto groundArmy = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsArmy(), ph->pos, obs, unitInfo.sight_range); 
		Units liftTargets;
		for (auto unit : groundArmy)
		{
			//TODO: move to outside the loop via caching
			auto unitAttr = (*allUnitInfo)[unit->unit_type].attributes;
			bool isLiftable = VectorHelpers::FoundInVector(unitAttr, Attribute::Massive);
			if (!isLiftable) liftTargets.push_back(unit);
		}
		if (liftTargets.size())
		{
			actions->UnitCommand(ph, ABILITY_ID::EFFECT_GRAVITONBEAM, groundArmy[0]);
		}
	}

}