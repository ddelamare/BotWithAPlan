#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common/Util.h>

class PickOffExpoGoal : public BaseAction
{
public:
	PickOffExpoGoal() : BaseAction() {
		this->BaseAction::name = "Pick Off Expo";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		// See how many expos we see
		double score = 0;
		auto enemyExpos = obs->GetUnits(sc2::Unit::Alliance::Enemy, IsTownHall());
		for (auto expo : enemyExpos)
		{
			score += DistanceSquared2D(expo->pos, state->EnemyBase);
		}
		score = score / 100;
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool queuedScouting = false;
		auto enemyExpos = obs->GetUnits(sc2::Unit::Alliance::Enemy, IsTownHall());
		auto point = Util::ToPoint3D(state->EnemyBase);
		std::sort(enemyExpos.begin(), enemyExpos.end(), Sorters::sort_by_distance(point));
		auto attackPoint = enemyExpos.back();

		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		actions->UnitCommand(army, ABILITY_ID::ATTACK, attackPoint->pos);

		return queuedScouting;
	}
};