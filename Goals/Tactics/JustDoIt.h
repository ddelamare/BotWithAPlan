#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"

class JustDoitGoal : public BaseAction
{
public:
	JustDoitGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->BaseAction::name = "Just Do It";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 2 * (obs->GetFoodArmy() / 50);
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
		state->ArmyManager->SetMode(&state->ArmyManager->battleGroups[0], BattleMode::Attack);

		return madeStaker;
	}
};