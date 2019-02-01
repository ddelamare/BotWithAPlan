#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Managers\ArmyManager.h"

class AllOutGoal : public BaseAction
{
	ThreatAnalyzer threatAnalyzer;
	bool hasSpoken = false;
public:
	AllOutGoal() : BaseAction() {
		this->BaseAction::name = "All In";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 50 * (threatAnalyzer.GetThreat(&state->threat) > 1.6);
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		auto visibleBuildings = obs->GetUnits(Unit::Alliance::Enemy, IsVisibleArmy());
		auto enemyStuff = obs->GetUnits(Unit::Alliance::Enemy);
		auto attackPoint = state->EnemyBase;
		if (visibleBuildings.size())
		{
			attackPoint = visibleBuildings[0]->pos;
		}
		else if (enemyStuff.size())
		{
			attackPoint = enemyStuff[0]->pos;
		}
		state->ArmyManager->SetTarget(&state->ArmyManager->battleGroups[0], attackPoint);
		if (!hasSpoken)
		{
			actions->SendChat("GERONIMO!!!");
			hasSpoken = true;
		}
		return madeStaker;
	}
};