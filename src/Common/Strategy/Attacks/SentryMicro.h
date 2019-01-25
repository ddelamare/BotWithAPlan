#pragma once
#include "Common/Util.h"
#include <vector>
using namespace sc2;

class SentryMicro
{
	UnitTypeData unitInfo;
	std::vector<UNIT_TYPEID> unitTypesToShield;
public:
	SentryMicro(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
SentryMicro::SentryMicro(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	auto info = &state->UnitInfo;
	unitInfo = (*info)[(int)UNIT_TYPEID::PROTOSS_SENTRY];
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::TERRAN_MARINE);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::TERRAN_MARAUDER);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::PROTOSS_ADEPT);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::ZERG_HYDRALISK);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::ZERG_MUTALISK);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::ZERG_ROACH);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::PROTOSS_VOIDRAY);
	unitTypesToShield.push_back(sc2::UNIT_TYPEID::PROTOSS_CARRIER);
}
void SentryMicro::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto sentries = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_SENTRY));

	for (auto unit : sentries)
	{
		auto targetsToShieldAgainst = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsUnits(unitTypesToShield), unit->pos, obs, 10);
		auto alliedUnits = Util::FindNearbyUnits(Unit::Alliance::Self, IsCombatUnit(), unit->pos, obs, 10);
		// If there are enemies around that are weak against guardian shield then use it
		// Only if there;s unit to protect
		if (targetsToShieldAgainst.size() >= 5 && alliedUnits.size() >= 5)
		{
			actions->UnitCommand(unit, ABILITY_ID::EFFECT_GUARDIANSHIELD);
		}
	}

}