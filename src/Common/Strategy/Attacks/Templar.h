#pragma once
#include "Common/Util.h"
#include "Common\Strategy\Attacks\UnitMicro.h"

#include <vector>
using namespace sc2;

class TemplarMicro : public UnitMicro
{
	UnitTypeData unitInfo;
	std::vector<UNIT_TYPEID> feedbackableUnits;
public:
	TemplarMicro(const sc2::ObservationInterface* obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
TemplarMicro::TemplarMicro(const sc2::ObservationInterface* obs, sc2::QueryInterface* query, GameState* state)
{
	auto info = &state->UnitInfo;
	unitInfo = (*info)[(int)UNIT_TYPEID::PROTOSS_HIGHTEMPLAR];
	feedbackableUnits.push_back(UNIT_TYPEID::TERRAN_MEDIVAC);
	feedbackableUnits.push_back(UNIT_TYPEID::TERRAN_GHOST);
	feedbackableUnits.push_back(UNIT_TYPEID::TERRAN_RAVEN);
	feedbackableUnits.push_back(UNIT_TYPEID::ZERG_INFESTOR);
	feedbackableUnits.push_back(UNIT_TYPEID::ZERG_VIPER);
	feedbackableUnits.push_back(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
}
void TemplarMicro::Execute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
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
		auto targetedClusters = std::set<int>();

		for (auto unit : templar)
		{
			int STORM_RADIUS = 3;
			auto feedbackTargets = Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsUnits(feedbackableUnits), unit->pos, obs, 10);
			auto enemyUnits = Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsEnemyArmy(), unit->pos, obs, 10);
			auto enemyClusters = Util::FindClusters(enemyUnits, STORM_RADIUS); //Find clusters the size of templar
			auto stormsActive = obs->GetEffects();

			if (unit->energy < 50)
			{
				templar_merge.push_back(unit);
			}
			else if (feedbackTargets.size())
			{
				for (auto u : feedbackTargets)
				{
					if (u->energy >75)
					{
						actions->UnitCommand(unit, ABILITY_ID::EFFECT_FEEDBACK, u);
					}
				}
			}
			if (enemyClusters.size())
			{
				std::sort(enemyClusters.begin(), enemyClusters.end(), Sorters::sort_by_cost(state));
				std::reverse(enemyClusters.begin(), enemyClusters.end());

				for (int i = 0; i < enemyClusters.size(); i++)
				{
					auto ecluster = enemyClusters[i];

					if (ecluster.second.size() < 3 || Util::GetUnitValues(ecluster.second, &state->UnitInfo) < 100)
						continue;

					// Find all clusters affected by storm
					for (auto storm : stormsActive)
					{
						if (Distance2D(storm.positions.front(), ecluster.first) < STORM_RADIUS)
						{
							targetedClusters.insert(i);
						}
					}

					if (targetedClusters.find(i) != targetedClusters.end())
						continue;

					auto cluster = enemyClusters[i];


					debug->DebugSphereOut(cluster.first, 2, Colors::Red);
					actions->UnitCommand(unit, ABILITY_ID::EFFECT_PSISTORM, cluster.first);
					targetedClusters.insert(i);
					break;

				}
			}
		}
	}

	// Morph the leftovers
	actions->UnitCommand(templar_merge, ABILITY_ID::MORPH_ARCHON);


}