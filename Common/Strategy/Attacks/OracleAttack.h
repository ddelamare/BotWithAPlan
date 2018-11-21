#pragma once
#include "Common/Util.h"
#include <vector>
using namespace sc2;

class OracleBeam
{
	UnitTypeData unitInfo;
	UnitTypes* allUnitInfo;
	std::vector<UNIT_TYPEID> priorityTargets;
public:
	OracleBeam(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
OracleBeam::OracleBeam(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	allUnitInfo = &state->UnitInfo;
	unitInfo = (*allUnitInfo)[(int)UNIT_TYPEID::PROTOSS_ORACLE];
	priorityTargets.push_back(UNIT_TYPEID::ZERG_QUEEN);
	priorityTargets.push_back(UNIT_TYPEID::TERRAN_MARINE);
	priorityTargets.push_back(UNIT_TYPEID::TERRAN_SCV);
	priorityTargets.push_back(UNIT_TYPEID::PROTOSS_PROBE);
	priorityTargets.push_back(UNIT_TYPEID::ZERG_DRONE);
}
void OracleBeam::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto oracles = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ORACLE));
	for (auto oracle : oracles)
	{
		/*auto enemyFlyersUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsFlying(), oracle->pos, obs, unitInfo.sight_range);
		if (enemyFlyersUnits.size()) return;*/
		auto groundArmy = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsUnits(priorityTargets), oracle->pos, obs, unitInfo.sight_range);
		Units liftTargets;
		if (groundArmy.size())
		{
			actions->UnitCommand(oracle, ABILITY_ID::BEHAVIOR_PULSARBEAMON);
		}
	}

}