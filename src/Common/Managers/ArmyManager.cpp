#include "Common/Managers/ArmyManager.h"
#include <iostream>
using namespace sc2;
void ArmyManager::ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	if (!HasThermalLance)
	{
		auto upgrades = obs->GetUpgrades();
		HasThermalLance = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::EXTENDEDTHERMALLANCE);
	}

	PartitionGroups(obs, query, action, state, debug);
	this->cachedHighPriorityEnemies = obs->GetUnits(IsHighPrioirtyEnemy());
	this->cachedEnemyArmy = obs->GetUnits(IsEnemyArmy());
	this->cachedEnemies = obs->GetUnits(IsEnemy());	// includes buildings

	auto units = obs->GetUnits(sc2::Unit::Alliance::Self, IsCombatUnit());
	auto clusters = Util::FindClusters(units, CLUSTER_DISTANCE_THRESHOLD_MIN);
	for (auto cluster : clusters)
	{
		debug->DebugTextOut("***" + std::to_string(Util::GetUnitValues(cluster.second, &state->UnitInfo)), cluster.first);
	}

	UpdateKnownEnemyPositions(obs, debug, state);

	for (auto group : battleGroups)
	{
		for (auto& u : group.units)
		{
			Color c = Colors::White;
			if (u->weapon_cooldown != 0)
			{
				c = Colors::Yellow;
			}

			debug->DebugBoxOut(u->pos - Point3D(u->radius / 2, u->radius / 2, -2), u->pos + Point3D(u->radius / 2, u->radius / 2, 2), c);
		}

		if (group.mode == BattleMode::Attack)
		{
			// Moves toward the target or attacks optimal units
			AttackTarget(&group, obs, query, action, state, debug);
			// Move units not in the cluster

			// For any cluster that's near enemy clusters that are bigger, retreat
			for (const auto& cluster : clusters)
			{
				if (ShouldUnitsRetreat(cluster, knownEnemyPresences, obs, query, state)) 
				{
					action->UnitCommand(cluster.second, ABILITY_ID::GENERAL_MOVE, state->StartingLocation);
				}
			}
		}
		else if (group.mode == BattleMode::Defend)
		{
			// Moves toward the target or attacks optimal units
			AttackTarget(&group, obs, query, action, state, debug);
		}
		else if (group.mode == BattleMode::Retreat)
		{
			action->UnitCommand(group.units, ABILITY_ID::GENERAL_MOVE, state->StartingLocation);
		}
		else if (group.mode == BattleMode::Harrass)
		{
			//TODO: Add special pathing and hit and retreat logic
			//if (IsClustered(&group, obs, query, action, state, debug))
			{
				if (HasTarget(&group))
				{
					SneakToTarget(&group, obs, query, action, state, debug);
				}
			}
			/*else
			{
				ClusterUnits(&group, obs, query, action, state, debug);
			}*/
		}
		debug->DebugSphereOut(Util::ToPoint3D(group.target, 8), 3, Colors::Green);
	}


	// TODO: Fan out better
	auto allUnits = obs->GetUnits(sc2::Unit::Alliance::Self, IsCombatUnit());
	auto toAvoid = obs->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTORPHASED));
	for (auto unit : allUnits)
	{
		auto nearbyEffects = Util::FindNearbyUnits(&toAvoid, unit->pos, obs, 5);

		if (!nearbyEffects.size()) continue;

		auto combinedVec = Point2D();
		auto absVec = Point2D();
		// Avoid effects
		for (auto effect : nearbyEffects)
		{
			// Find a safe spot by doing some vector addition
			Point2D vecToEnemy = effect->pos - unit->pos;
			Normalize2D(vecToEnemy);
			absVec += vecToEnemy;
		}

		// Rotate 90
		auto vecAway = -1 * absVec;
		Normalize2D(vecAway);

		vecAway *= 2;

		action->UnitCommand(unit, sc2::ABILITY_ID::GENERAL_MOVE, unit->pos + vecAway);
	}
}

void ArmyManager::AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	//Send attack command and choose targets

	auto averagePoint = Util::GetAveragePoint(group->units);
	auto enemyUnits = Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsHighPrioirtyEnemy(), averagePoint, obs, 20);
	// No High Priority? Then look for all
	if (enemyUnits.size() == 0)
	{
		enemyUnits = Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsEnemyArmy(), averagePoint, obs, 20);
	}

	Units unitsToMove;
	float minSpeed = 0;
	for (auto unit : group->units)
	{
		const Unit* enemyUnit = nullptr; 
		if (unit->weapon_cooldown == 0 && !VectorHelpers::FoundInVector(nonAutoAttackUnits, unit->unit_type) && (enemyUnit = FindOptimalTarget(unit, obs, query, state))) // If a enemy is near and can fire, target them
		{
			action->UnitCommand(unit, ABILITY_ID::ATTACK, enemyUnit);
		}
		else if (VectorHelpers::FoundInVector(backLineUnits, unit->unit_type) && enemyUnits.size())
		{
			//move unit to back of cluster
			auto unitType = &state->UnitInfo[unit->unit_type];
			float range = GetUnitTypeRange(unitType);

			auto closestUnit = Util::FindClosetOfType(this->cachedEnemyArmy, unit->pos, obs, query, false);
			
			// Keep units in the back, but also in searchRangeMod		
			auto vectorAway = closestUnit->pos - unit->pos;
			vectorAway.z = 0;
			Normalize3D(vectorAway);
			vectorAway *= -1 * (range + 1);
			auto distanceToKitePoint = DistanceSquared2D(closestUnit->pos + vectorAway, closestUnit->pos);
			auto distanceAwayFromKitePoint(DistanceSquared2D(unit->pos, closestUnit->pos));
			// Move away if it's further than the midpoint between the units
			if (distanceToKitePoint > distanceAwayFromKitePoint)
			{
				action->UnitCommand(unit, ABILITY_ID::GENERAL_MOVE, closestUnit->pos + vectorAway);
				debug->DebugLineOut(unit->pos, closestUnit->pos + vectorAway, Colors::Teal);
			}
		}
		else if (abs(unit->pos.x - group->target.x) > CLUSTER_MOVE_THRESHOLD && abs(unit->pos.y - group->target.y) > CLUSTER_MOVE_THRESHOLD)
		{
			unitsToMove.push_back(unit);
		}
	}

	if (HasTarget(group))
	{
		action->UnitCommand(unitsToMove, ABILITY_ID::ATTACK, group->target);
	}

}

void ArmyManager::RequestAction(Point2D target, BattleMode action)
{
	// TODO: add to request queue and partition accordingly
	this->requestedActions.push_back(std::make_pair(target, action));
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
		battleGroups[1].target = state->EnemyBase;
	}

	auto armyUnits = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());

	// If there are no new actions, add all new units to a group
	if (!requestedActions.size())
	{
		Units groupunits;
		for (auto &group : battleGroups)
		{
			// remove dead units
			group.units.erase(std::remove_if(group.units.begin(), group.units.end(), [](const Unit* u) { return !u->is_alive; }), group.units.end());
			groupunits.insert(groupunits.end(), group.units.begin(), group.units.end());
		}

		Units missing = VectorHelpers::FindMissingInVector(armyUnits, groupunits);

		battleGroups[0].units.insert(battleGroups[0].units.end(), missing.begin(), missing.end());

		return;
	}

	auto isHarrassUnit = IsUnit(UNIT_TYPEID::PROTOSS_ORACLE);

	for (auto& group : battleGroups)
	{
		group.units.clear();

		group.target = requestedActions[0].first;
		group.mode = requestedActions[0].second;

		for (auto unit : armyUnits)
		{
			if (group.mode != BattleMode::Harrass || isHarrassUnit(*unit))
			{
				group.units.push_back(unit);
			}
		}
	}

	requestedActions.clear();
}
void ArmyManager::SneakToTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug)
{
	if (group->units.size() <= 1) return;
	auto averagePoint = Util::GetAveragePoint(group->units);
	debug->DebugSphereOut(averagePoint, 5);
	Units unitsToMove;
	auto xTarget = 0;
	auto yTarget = 0;
	Point2D point;
	if (group->target.x < obs->GetGameInfo().playable_max.x / 2.0)
		xTarget = obs->GetGameInfo().playable_min.x;
	else
		xTarget = obs->GetGameInfo().playable_max.x;
	if (group->target.y < obs->GetGameInfo().playable_max.y / 2.0)
		yTarget = obs->GetGameInfo().playable_min.y;
	else
		yTarget = obs->GetGameInfo().playable_max.y;

	if (abs(averagePoint.x - xTarget) > 5)
		point = Point2D(xTarget, averagePoint.y);
	else if (abs(averagePoint.y - yTarget) > 5)
		point = Point2D(averagePoint.x, yTarget);
	if (Distance2D(averagePoint, group->target) < 20)
	{
		point = group->target;
	}

	action->UnitCommand(group->units, ABILITY_ID::ATTACK, point);

}

// Gets the range of the units weapon with some exceptions
int ArmyManager::GetUnitTypeRange(UnitTypeData* unitType)
{
	float range = 1;
	if (unitType->weapons.size())
	{
		//TODO: Find searchRangeMod based on eligibile targets ie air vs ground
		range = unitType->weapons[0].range;
	}
	if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_DISRUPTOR)
	{
		// Allow disruptors to get a little closer
		// TODO: only if they are not on cooldown
		range = Constants::DISRUPTOR_RANGE - 1;
	}
	else if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_COLOSSUS && HasThermalLance)
	{
		range = Constants::COLOSSUS_EXTENDED_RANGE;
	}
	else if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_CARRIER)
	{
		range = Constants::CARRIER_RANGE;
	}
	else if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)
	{
		range = Constants::STORM_RANGE;
	}
	else if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_SENTRY)
	{
		range = 5;
	}
	else if (unitType->unit_type_id == UNIT_TYPEID::PROTOSS_VOIDRAY)
	{
		range = 6;
	}
	return range;
}

const Unit* ArmyManager::FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query, GameState* state)
{
	auto unitData = &state->UnitInfo;
	if (!unitData->size()) return nullptr;
	auto unitType = (*unitData)[unit->unit_type];
	auto searchRangeMod = 1.3;
	if (unitType.weapons.size())
	{
		float range = GetUnitTypeRange(&unitType);
		// Prevent long range units from going too deep and dying.
		if (range > 8)
		{
			searchRangeMod = 1.1;
		}
		auto nearbyEnemies = Util::FindNearbyUnits(&this->cachedHighPriorityEnemies, unit->pos, obs, range * searchRangeMod);
		bool isHighPriority = true;
		if (nearbyEnemies.size() == 0)
		{
			nearbyEnemies = Util::FindNearbyUnits(&this->cachedEnemyArmy, unit->pos, obs, range * searchRangeMod);
			isHighPriority = false;
		}
		double minPercent = DBL_MAX;
		const Unit* weakestUnit = nullptr;

		// Sort by tag to prevent units from switching too quickly between units with the same health percentage. Otherwise, they change targets so much they can't attack.
		//std::sort(nearbyEnemies.begin(), nearbyEnemies.end(), Sorters::sort_by_tag());
		// DIstance is a beter metric for ranged units and it reasonably unique.
		std::sort(nearbyEnemies.begin(), nearbyEnemies.end(), Sorters::sort_by_distance(unit->pos));

		// Units needing to get sniped RIGHT NOW
		for (auto eu : nearbyEnemies)
		{
			// Infestors must die
			if (IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK)(*eu))
			{
				weakestUnit = eu;
			}
		}

		// If there are no priority targets, then find unit with least health.
		if (!weakestUnit)
		{
			for (auto eu : nearbyEnemies)
			{
				auto percentHealth = (eu->health + eu->shield) / (eu->health_max + eu->shield_max);
				// Find weakest unit below full health, unless it's high priority, in which case include full health. This is so we can target pylons even if there's observers or something near by
				if (percentHealth < minPercent && (isHighPriority || percentHealth < 1.0) && Util::CanAttack(unit, eu, state))
				{
					weakestUnit = eu;
					minPercent = percentHealth;
				}
			}
		}
		auto nearbyBuildings = Util::FindNearbyUnits(&this->cachedEnemies, unit->pos, obs, range * searchRangeMod * 3);
		std::sort(nearbyBuildings.begin(), nearbyBuildings.end(), Sorters::sort_by_tag());

		if (!weakestUnit)
		{
			// Attack workers before buildings
			for (auto eu : nearbyBuildings)
			{
				if (IsWorker()(*eu))
				{
					weakestUnit = eu;
				}
			}
		}
		if (!weakestUnit)
		{
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
	// These units will also kite
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_COLOSSUS);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_DISRUPTOR);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_VOIDRAY);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_CARRIER);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_TEMPEST);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_SENTRY);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_STALKER);
	backLineUnits.push_back(UNIT_TYPEID::PROTOSS_IMMORTAL);
	backLineUnits.push_back(UNIT_TYPEID::TERRAN_MARINE);


	nonAutoAttackUnits.push_back(UNIT_TYPEID::PROTOSS_CARRIER);
	nonAutoAttackUnits.push_back(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
	nonAutoAttackUnits.push_back(UNIT_TYPEID::PROTOSS_DISRUPTOR);
	nonAutoAttackUnits.push_back(UNIT_TYPEID::PROTOSS_SENTRY);

	threatAnalyzer = ThreatAnalyzer();
	HasThermalLance = false;
}

int ENEMY_CLUSTER_SEARCH_RANGE = 20;
// This method should take a cluster of friendly units near a cluster of enemy units and see if they should retreat.
bool ArmyManager::ShouldUnitsRetreat(std::pair<Point3D, Units> cluster, std::vector<KnownEnemyPresence*> enemyClusters, const ObservationInterface* obs, QueryInterface* query,  GameState* state)
{
	int enemyCostsInRange = 0;
	for (auto ecluster : enemyClusters)
	{
		if (Distance2D(cluster.first, ecluster->location) <= ENEMY_CLUSTER_SEARCH_RANGE)
		{
			enemyCostsInRange += ecluster->unitValue;
		}
	}

	int friendlyUnitsCost = Util::GetUnitValues(cluster.second, &state->UnitInfo);

	auto closestTownHall = Util::FindClosetOfType(obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall()), cluster.first, obs, query, false);
	bool isAwayFromHome = false;
	if (closestTownHall) {
		auto disToTownHall = Distance2D(closestTownHall->pos, cluster.first);

		// Retreat unless we are at home
		isAwayFromHome = disToTownHall > 20;
	}

	return isAwayFromHome && (friendlyUnitsCost < (enemyCostsInRange * .8));
}


void ArmyManager::UpdateKnownEnemyPositions(const ObservationInterface* obs, DebugInterface* debug, GameState* state)
{
	auto enemyClusters = Util::FindClusters(this->cachedEnemyArmy, CLUSTER_DISTANCE_THRESHOLD_MIN);

	//Steps
	/* KEP = Known Enemy Position
	* Prune old KEP and dead units
	* Prune KEP that have visible points, but no nearby units
	* Loop through visible clusters. If near KEP, move KEP center and update units and cost, and refresh last seen loop
	*	* If cluster not near KEP, create a new KEP
	*/

	int MAX_PRESENCE_AGE = 1000;
	int loop = obs->GetGameLoop();

	// Prune old KEP and dead units. Iterate backwards to remove
	for (int i = knownEnemyPresences.size() - 1; i >= 0; i--)
	{
		// Prune old KEPs
		if (loop - knownEnemyPresences[i]->lastSeenLoop > MAX_PRESENCE_AGE)
		{
			VectorHelpers::RemoveFromVector(&knownEnemyPresences, knownEnemyPresences[i]);
			continue;
		}

		// Prune visible locations without units
		if (obs->GetVisibility(knownEnemyPresences[i]->location) == sc2::Visibility::Visible)
		{
			bool hasNearbyEnemyUnits = false;
			for (auto cluster : enemyClusters)
			{
				if (Distance2D(knownEnemyPresences[i]->location, cluster.first) < CLUSTER_DISTANCE_THRESHOLD_MIN)
				{
					hasNearbyEnemyUnits = true;
					break;
				}
			}
			if (!hasNearbyEnemyUnits)
			{
				VectorHelpers::RemoveFromVector(&knownEnemyPresences, knownEnemyPresences[i]);
				continue;
			}
		}

		// Remove dead units from KEP
		for (int j =  knownEnemyPresences[i]->enemies.size() -1; j >= 0; j--)
		{
			auto unit = knownEnemyPresences[i]->enemies[j];
			if (!unit->is_alive)
			{
				VectorHelpers::RemoveFromVector(&knownEnemyPresences[i]->enemies, unit);
			}
		}
	}

	// If the KEP moved, update the location. If no KEP is nearby, make a new one
	for (auto cluster : enemyClusters)
	{
		double minDis = DBL_MAX;
		KnownEnemyPresence* closestKep = nullptr;
		for (auto kep : knownEnemyPresences)
		{
			// Find nearest cluster that was not updated this loop
			if (kep->lastSeenLoop != loop && Distance2D(kep->location, cluster.first) < minDis)
			{
				minDis = Distance2D(kep->location, cluster.first);
				closestKep = kep;
			}
		}

		bool newKep = false;
		// If no KEP found, make new KEP
		if (!closestKep)
		{
			closestKep = new KnownEnemyPresence();
			newKep = true;
			knownEnemyPresences.insert(knownEnemyPresences.end(), closestKep);
		}

		// Create or Update KEP
		if (newKep || minDis < CLUSTER_DISTANCE_THRESHOLD_MIN)
		{
			// Update KEP with cluster
			closestKep->enemies = cluster.second;
			closestKep->lastSeenLoop = loop;
			closestKep->location = cluster.first;
			closestKep->unitValue = std::max(Util::GetUnitValues(cluster.second, &state->UnitInfo), closestKep->unitValue);
			continue;
		}
	}

	for (auto kep : knownEnemyPresences)
	{
		debug->DebugTextOut("***" + std::to_string(kep->unitValue), kep->location, Colors::Red);
	}
}