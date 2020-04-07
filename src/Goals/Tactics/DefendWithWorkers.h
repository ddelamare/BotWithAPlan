#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"
#include <algorithm>

class DefendWithUnitsGoal : public BaseAction
{
	int numWorkers = 0;
public:
	DefendWithUnitsGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->BaseAction::name = "Defend with Workers";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto enemyUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsEnemyGroundArmy(), Util::ToPoint3D(state->StartingLocation), obs, 20);
		if (enemyUnits.size())
		{
			numWorkers = 0;
			if (enemyUnits.size() > obs->GetFoodArmy())
				return 10;
		}
		else
		{
			// No enemy army, find if theres a harrassing worker
			auto enemyWorkers = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsNotFlying(), Util::ToPoint3D(state->StartingLocation), obs, 20);
			numWorkers = enemyWorkers.size();
			if (enemyWorkers.size())
			{
				return 5;
			}
		}
			return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool attacked = false;
		auto enemyUnits = Util::FindNearbyUnits(IsEnemy(), Util::ToPoint3D(state->StartingLocation), obs, 40);
		auto targetPoint = Util::GetAveragePoint(enemyUnits);
		// If it's not just workers, send everything
		if (numWorkers == 0)
		{
			auto army = obs->GetUnits(Unit::Alliance::Self);
			actions->UnitCommand(army, ABILITY_ID::ATTACK, targetPoint);
		}
		else
		{
			// If it's only a few workers, respond in kind
			auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
			numWorkers -= army.size();
			auto workers = obs->GetUnits(Unit::Alliance::Self, IsWorker());
			auto attackingWorkers = Util::GetUnitsWithOrder(workers, ABILITY_ID::ATTACK);
			if (attackingWorkers.size())
			{
				int numLeftWorkers = max(min((int)attackingWorkers.size() - 1, numWorkers),0);

				army.insert(army.begin(), attackingWorkers.begin(), attackingWorkers.begin() + numLeftWorkers);
				numWorkers -= attackingWorkers.size();
				for (int i = 0; i < attackingWorkers.size(); i++)
				{
					VectorHelpers::RemoveFromVector(&workers, attackingWorkers[i]);
				}
			}

			if (numWorkers > 0 && workers.size())
			{
				// Send workers as backup. TODO: Grab closest and idle first
				numWorkers = min((int)workers.size() - 1, numWorkers);
				// This sends n+1 or all if workers if less
				army.insert(army.begin(), workers.begin(), workers.begin() + numWorkers);
			}
			actions->UnitCommand(army, ABILITY_ID::ATTACK, targetPoint);
		}

		return false;
	}
};