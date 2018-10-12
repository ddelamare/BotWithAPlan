#pragma once
#include "Common/Util.h"
using namespace sc2;

class TemplarMicro
{
	UnitTypeData unitInfo;
public:
	TemplarMicro(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
TemplarMicro::TemplarMicro(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	auto info = &state->UnitInfo;
	unitInfo = (*info)[(int)UNIT_TYPEID::PROTOSS_HIGHTEMPLAR];
}
void TemplarMicro::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto templar = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR));
	auto upgrades = obs->GetUpgrades();
	auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::PSISTORMTECH);
	Units templar_merge;
	if (templar.size() > 1 && !hasUpgrade) {
		for (int i = 0; i < 2; ++i) {
			templar_merge.push_back(templar.at(i));
		}
	}
	else
	{
		for (auto unit : templar)
		{
			auto feedbackTargets = Util::FindNearbyUnits(IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC), unit->pos, obs, 10);
			auto enemyUnits = Util::FindNearbyUnits(IsEnemyArmy(), unit->pos, obs, 10);
			if (unit->energy < 50)
			{
				templar_merge.push_back(unit);
			}
			else if (feedbackTargets.size())
			{
				for (auto u : feedbackTargets)
				{
					if (u->energy > 100)
					{
						actions->UnitCommand(unit, ABILITY_ID::EFFECT_FEEDBACK, u);
					}
				}
			}
			if (enemyUnits.size())
			{
				auto targetPoint = Util::GetAveragePoint(enemyUnits);
				actions->UnitCommand(unit, ABILITY_ID::EFFECT_PSISTORM, targetPoint);
				break;
			}
		}
	}

	// Morph the leftovers
	actions->UnitCommand(templar_merge, ABILITY_ID::MORPH_ARCHON);


}