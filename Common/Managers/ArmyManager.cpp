#include "Common/Managers/ArmyManager.h"
using namespace sc2;
void ArmyManager::ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	PartitionGroups(obs, query, action, state, debug);
	this->cachedEnemyArmy = obs->GetUnits(IsEnemyArmy());
	this->cachedEnemies = obs->GetUnits(IsEnemy());	// includes buildings
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
		else if (group.mode == BattleMode::Harrass)
		{
			//TODO: Add special pathing and hit and retreat logic
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
	Units unitsToMove;
	for (auto unit : group->units)
	{
		//if (abs(unit->pos.x - averagePoint.x) > CLUSTER_MOVE_THRESHOLD || abs(unit->pos.y - averagePoint.y) > CLUSTER_MOVE_THRESHOLD)
		{
			unitsToMove.push_back(unit);
		}
	}
	action->UnitCommand(unitsToMove, ABILITY_ID::ATTACK, averagePoint);
}
void ArmyManager::AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	//Send attack command and choose targets
	//action->UnitCommand(group->units, ABILITY_ID::ATTACK, group->target);

	auto averagePoint = Util::GetAveragePoint(group->units);
	auto enemyUnits = Util::FindNearbyUnits(IsEnemyArmy(), averagePoint, obs, 20);
	// This point should be where the sqishy units go. AKA not the front lines
	auto enemyAvgPoint = Util::GetAveragePoint(enemyUnits);
	auto vectorAway = enemyAvgPoint = averagePoint - enemyAvgPoint;
	Normalize3D(vectorAway);
	Units unitsToMove;
	for (auto unit : group->units)
	{
		auto enemyUnit = FindOptimalTarget(unit, obs, query, state);
		if (enemyUnit)
		{
			action->UnitCommand(unit, ABILITY_ID::ATTACK, enemyUnit);
		}
		else if (VectorHelpers::FoundInVector(backLineUnits, unit->unit_type) && enemyUnits.size())
		{
			//move unit to back of cluster
			auto unitType = state->UnitInfo[unit->unit_type];
			double range = 1;
			if (unitType.weapons.size())
			{
				//TODO: Find range based on eligibile targets ie air vs ground
				range = unitType.weapons[0].range;
			}
			if (unitType.unit_type_id == UNIT_TYPEID::PROTOSS_DISRUPTOR)
			{
				range = 8; // Disruptor ability range. Weapon range is 1 for some reason
			}
			// Keep units in the back, but also in range
			action->UnitCommand(unit, ABILITY_ID::ATTACK, enemyAvgPoint + (range * vectorAway));
			debug->DebugSphereOut(enemyAvgPoint, 3, Colors::Teal);
		}
		else //if (abs(unit->pos.x - group->target.x) > CLUSTER_MOVE_THRESHOLD && abs(unit->pos.y - group->target.y) > CLUSTER_MOVE_THRESHOLD)
		{
			unitsToMove.push_back(unit);
		}
	}
	action->UnitCommand(unitsToMove, ABILITY_ID::ATTACK, group->target);

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
	if (battleGroups.size() < 2)
	{
		battleGroups.push_back(BattleGroup());
		battleGroups[0].mode = BattleMode::Defend; 
		battleGroups.push_back(BattleGroup());
		battleGroups[1].mode = BattleMode::Harrass;
	}
	auto armyUnits = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
	auto isOracle = IsUnit(UNIT_TYPEID::PROTOSS_ORACLE);
	for (auto& bg : battleGroups)
	{
		bg.units.clear();
	}
	for (auto unit : armyUnits)
	{
		if (isOracle(*unit))
		{
			battleGroups[1].units.push_back(unit);
		}
		else
		{
			battleGroups[0].units.push_back(unit);
		}
	}
}

const Unit* ArmyManager::FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query, GameState* state)
{
	auto unitData = &state->UnitInfo;
	if (!unitData->size()) return nullptr;
	auto unitType = (*unitData)[unit->unit_type];	
	if (unitType.weapons.size())
	{
		auto nearbyEnemies = Util::FindNearbyUnits(&this->cachedEnemyArmy, unit->pos, obs, unitType.weapons[0].range * 2.0);
		double minPercent = DBL_MAX;
		const Unit* weakestUnit = nullptr;
		for (auto eu : nearbyEnemies)
		{
			auto percentHealth = (eu->health + eu->shield) / (eu->health_max + eu->shield_max);
			if (percentHealth < minPercent)
			{
				weakestUnit = eu;
				minPercent = percentHealth;
			}
		}
		if (!weakestUnit)
		{
		   auto nearbyBuildings = Util::FindNearbyUnits(&this->cachedEnemies, unit->pos, obs, unitType.weapons[0].range * 2.0);
		   int pylons = 0;
		   int poweredBuildings = 0;
		   auto isPylon = IsUnit(UNIT_TYPEID::PROTOSS_PYLON);
		   const Unit* pylon = nullptr;
		   for (auto eu : nearbyBuildings)
		   {
			   if (isPylon(*eu))
			   {
				   pylons++;
				   pylon = eu;
			   }
			   else if (eu->is_powered)
			   {
				   poweredBuildings++;
			   }
		   }
		   // if there are more than twice the number of buildings as pylons, target the pylons
		   if ((pylons * 2) <= poweredBuildings)
		   {
			   weakestUnit = pylon;
		   }
		}
		return weakestUnit;
	}
	else
	{
		return nullptr;
	}
}

ArmyManager::ArmyManager()
{
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_COLOSSUS);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_DISRUPTOR);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_VOIDRAY);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_CARRIER);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_SENTRY);
}