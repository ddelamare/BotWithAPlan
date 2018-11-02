#pragma once
#include "Common/Util.h"
using namespace sc2;

class DisruptorAttack
{
	UnitTypeData unitInfo;
public:
	DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
DisruptorAttack::DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query, GameState* state)
{
	auto info = &state->UnitInfo;
	unitInfo = (*info)[(int)UNIT_TYPEID::PROTOSS_DISRUPTOR];
}
void DisruptorAttack::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto disruptors = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTOR));
	for (auto unit : disruptors)
	{
		auto enemyUnits = Util::FindNearbyUnits(IsEnemyGroundArmy(), unit->pos, obs, 10);

		if (enemyUnits.size())
		{
            auto targetPoint = Util::GetAveragePoint(enemyUnits);
            // Subtract start minus end to get a vector away from the center
            debug->DebugSphereOut(targetPoint, 2, Colors::Red);
            actions->UnitCommand(unit, ABILITY_ID::EFFECT_PURIFICATIONNOVA, targetPoint);
		}
		else
		{
			enemyUnits = Util::FindNearbyUnits(IsEnemyBuilding(), unit->pos, obs, 10);
			if (enemyUnits.size())
			{
				auto targetPoint = Util::GetAveragePoint(enemyUnits);
				// Subtract start minus end to get a vector away from the center
				debug->DebugSphereOut(targetPoint, 2, Colors::Blue);
				actions->UnitCommand(unit, ABILITY_ID::EFFECT_PURIFICATIONNOVA, targetPoint);
			}
		}
	}


	//Micro the orbs
	auto orbs = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTORPHASED));
	for (auto unit : orbs)
	{
		auto enemyUnits = Util::FindNearbyUnits(IsEnemyGroundArmy(), unit->pos, obs, 10);
		if (enemyUnits.size())
		{
			auto targetPoint = Util::GetAveragePoint(enemyUnits);
			actions->UnitCommand(unit, ABILITY_ID::ATTACK, targetPoint);
		}
		else
		{
			// No enemys nearby? Kill whatever
			auto enemys = Util::FindNearbyUnits(IsEnemy(), unit->pos, obs, 10);
			if (enemys.size())
			{
				auto targetPoint = Util::GetAveragePoint(enemyUnits);
				actions->UnitCommand(unit, ABILITY_ID::ATTACK, targetPoint);
			}
		}
	}

}