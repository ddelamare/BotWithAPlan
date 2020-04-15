#pragma once
#include "Common/Util.h"
#include "Common\Strategy\Attacks\UnitMicro.h"
#include "sc2lib\sc2_search.h"
#include <iostream>
using namespace sc2;

class DisruptorAttack : public UnitMicro
{
	UnitTypeData unitInfo;
	std::map<Tag, long> OrbMap;
public:
	DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
DisruptorAttack::DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	auto info = &state->UnitInfo;
	unitInfo = (*info)[(int)UNIT_TYPEID::PROTOSS_DISRUPTOR];
	OrbMap = std::map<Tag, long>();
}
void DisruptorAttack::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto disruptors = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTOR));
	auto enemyTargets = obs->GetUnits(Unit::Alliance::Enemy, IsEnemyGroundArmy());
	auto enemyClusters = Util::FindClusters(enemyTargets, 3.5); //Find clusters the size of disruptor blast radius

	//Draw enemy clusters
	for (auto cluster : enemyClusters)
	{
		debug->DebugSphereOut(cluster.first, .5, Colors::Yellow);
	}

	for (auto unit : disruptors)
	{
		//auto enemyUnits = Util::FindNearbyUnits(IsEnemyGroundArmy(), unit->pos, obs, Constants::DISRUPTOR_RANGE);

		if (enemyClusters.size())
		{
			for (auto cluster : enemyClusters)
			{
				auto dis = Distance2D(unit->pos, cluster.first);
				if ( dis < Constants::DISRUPTOR_RANGE + 1)
				{
					debug->DebugSphereOut(cluster.first, 2, Colors::Red);
					actions->UnitCommand(unit, ABILITY_ID::EFFECT_PURIFICATIONNOVA, cluster.first);
					break;
				}

			}
		}
		else
		{
			auto enemyUnits = Util::FindNearbyUnits(IsEnemyBuilding(), unit->pos, obs, 10);
			if (enemyUnits.size())
			{
				auto targetPoint = Util::GetAveragePoint(enemyUnits);
				// Subtract start minus end to get a vector away from the center
				debug->DebugSphereOut(targetPoint, 2, Colors::Blue);
				actions->UnitCommand(unit, ABILITY_ID::EFFECT_PURIFICATIONNOVA, targetPoint);
			}
		}
	}

	//TODO: Sort clusters by estimated minerals killed

	//Micro the orbs
	auto orbs = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTORPHASED));

	//TODO: find cluster in radius
	//TODO: factor in move speed and time till explosion
	//TODO: spread out orbs to different clusters

	auto orbsDirectedAlready = Units();
	int ORB_TIME_TO_LIVE = 47; // 47 alive frames. Achieved by testing.	Also near frames 22.1 frames per sec times 2.1 ball life span
	long gameLoop = obs->GetGameLoop();
	// Assign orbs by best cluster first
	for (auto cluster : enemyClusters)
	{
		const Unit* closestOrb = nullptr;
		float distance = std::numeric_limits<float>::max();
		for (auto orb : orbs)
		{
			// Keep Orb Map up to date
			if (OrbMap[orb->tag] == 0)
			{
				OrbMap[orb->tag] = obs->GetGameLoop(); // Mark orb create frame
			}

			if (VectorHelpers::FoundInVector(orbsDirectedAlready, orb))
			{
				continue;
			}
			auto d = Distance2D(orb->pos, cluster.first);
			auto firstFrame = OrbMap[orb->tag];
			auto timeToLive = 1 - (((float)gameLoop - firstFrame) / ORB_TIME_TO_LIVE); // Get percent lifespan left
			float ORB_SEARCH_DISTANCE = Constants::DISRUPTOR_RANGE * timeToLive;
			if (d <= ORB_SEARCH_DISTANCE && d < distance)
			{
				distance = d;
				closestOrb = orb;
			}
		}
		if (closestOrb)
		{
			actions->UnitCommand(closestOrb, ABILITY_ID::ATTACK, cluster.first);
			orbsDirectedAlready.push_back(closestOrb);
		}
	}

	// Direct any undirected orbs
	for (auto orb : orbs)
	{

		if (VectorHelpers::FoundInVector(orbsDirectedAlready, orb))
		{
			continue;
		}

		auto enemyUnits = Util::FindNearbyUnits(IsEnemyGroundArmy(), orb->pos, obs, 10);
		if (enemyUnits.size())
		{
			auto targetPoint = Util::GetAveragePoint(enemyUnits);
			actions->UnitCommand(orb, ABILITY_ID::ATTACK, targetPoint);
		}
		else
		{
			// No enemys nearby? Kill whatever
			auto enemys = Util::FindNearbyUnits(IsEnemy(), orb->pos, obs, 10);
			if (enemys.size())
			{
				auto targetPoint = Util::GetAveragePoint(enemyUnits);
				actions->UnitCommand(orb, ABILITY_ID::ATTACK, targetPoint);
			}
		}
	}

}