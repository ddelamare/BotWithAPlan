#pragma once
#include "Common/Util.h"
using namespace sc2;

class BlinkStalker
{
	UnitTypeData unitInfo;
	const int STALKER_SHIELD_THRESH = 10;
public:
	BlinkStalker(const sc2::ObservationInterface *obs, sc2::QueryInterface* query);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
BlinkStalker::BlinkStalker(const sc2::ObservationInterface *obs, sc2::QueryInterface* query)
{
	auto info = obs->GetUnitTypeData();
	unitInfo = info[(int)UNIT_TYPEID::PROTOSS_STALKER];
}
void BlinkStalker::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto stalkers = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_STALKER));
	for (auto unit : stalkers)
	{
		// If unit is below shield threshold, blink back
		if (unit->shield <= this->STALKER_SHIELD_THRESH)
		{
			// Find nearby units and blink away
			auto enemyUnits = Util::FindNearbyUnits(IsEnemyArmy(), unit->pos, obs, 10);
			if (enemyUnits.size())
			{
				auto vectorAway = Util::GetAveragePoint(enemyUnits);
				// Subtract start minus end to get a vector away from the center
				vectorAway = (unit->pos * 2) - vectorAway;
				debug->DebugSphereOut(vectorAway, 2, Colors::Red);
				actions->UnitCommand(unit, ABILITY_ID::EFFECT_BLINK, vectorAway);
			}
		}
	}

}