#include "Common/Managers/ArmyManager.h"
using namespace sc2;
void ArmyManager::ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	PartitionGroups(obs, query, action, state, debug);
	for (auto group : battleGroups)
	{
		if (group.mode == BattleMode::Attack)
		{
			if (IsClustered(&group, obs, query, action, state, debug))
			{
				if (HasTarget(&group))
				{
					AttackTarget(&group, obs, query, action, state, debug);
				}
			}
			else
			{
				ClusterUnits(&group, obs, query, action, state, debug);
			}
		}
		else if (group.mode == BattleMode::Defend)
		{
			if (HasTarget(&group))
			{
				AttackTarget(&group, obs, query, action, state, debug);
			}
		}
		else if (group.mode == BattleMode::Retreat)
		{
			action->UnitCommand(group.units, ABILITY_ID::MOVE, state->StartingLocation);
		}
	}
}

// Checks if a group is close enough together
bool ArmyManager::IsClustered(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	if (group->units.size() <= 5) return false;

	//see if units are clustered well enough
	auto averagePoint = Util::GetAveragePoint(group->units);
	int outsideOfCluster = 0;
	for (auto unit : group->units)
	{
		if (Distance2D(unit->pos, averagePoint) > CLUSTER_DISTANCE_THRESHOLD)
		{
			outsideOfCluster++;
		}
	}
	auto clusterRatio = (double)outsideOfCluster / (double)(group->units.size());
	return clusterRatio <= CLUSTER_PERCENT_THRESHOLD;
}
void ArmyManager::ClusterUnits(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	//TODO: Based on where they're going, meet up in a sane spot.
	if (group->units.size() <= 1) return;
	auto averagePoint = Util::GetAveragePoint(group->units);
	debug->DebugSphereOut(averagePoint, 5);
	//if (HasTarget(group))
	//{
	//	averagePoint = (averagePoint + Util::ToPoint3D(group->target)) / 2);
	//}
	action->UnitCommand(group->units, ABILITY_ID::ATTACK, averagePoint);
}
void ArmyManager::AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	//TODO: Send attack command and choose targets
	action->UnitCommand(group->units, ABILITY_ID::ATTACK, group->target);
	for (auto unit : group->units)
	{
		auto enemyUnit = FindOptimalTarget(unit, obs, query);
		if (enemyUnit)
		{
			action->UnitCommand(unit, ABILITY_ID::ATTACK, enemyUnit);
		}
	}
}
void ArmyManager::SetTarget(BattleGroup* group, Point2D location)
{
	group->target = location;
}
void ArmyManager::SetMode(BattleGroup* group, BattleMode mode)
{
	group->mode = mode;
}
bool ArmyManager::HasTarget(BattleGroup* group)
{
	return (group->target.x > 0 && group->target.y > 0);
}
void ArmyManager::PartitionGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{

	//TODO: make defense groups
	if (battleGroups.size() == 0)
	{
		battleGroups.push_back(BattleGroup());
		battleGroups[0].mode = BattleMode::Defend;
		//battleGroups[0].target = state->EnemyBase;
	}
	battleGroups[0].units.clear();
	auto armyUnits = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
	battleGroups[0].units.insert(battleGroups[0].units.end(), armyUnits.begin(), armyUnits.end());
}

const Unit* ArmyManager::FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query)
{
	auto unitData = obs->GetUnitTypeData();
	if (!unitData.size()) return nullptr;
	auto unitType = unitData[unit->unit_type];	if (unitType.weapons.size())
	{
		auto enemyUnits = Util::FindNearbyUnits(IsEnemyArmy(), unit->pos, obs, unitType.weapons[0].range * 1.5);
		double minPercent = DBL_MAX;
		const Unit* weakestUnit = nullptr;
		for (auto eu : enemyUnits)
		{
			auto percentHealth = (eu->health + eu->shield) / (eu->health_max + eu->shield_max);
			if (percentHealth < minPercent)
			{
				weakestUnit = eu;
				minPercent = percentHealth;
			}
		}
		return weakestUnit;
	}
	else
	{
		return nullptr;
	}
}

