#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Managers\ArmyManager.h"

class AllOutGoal : public BaseAction
{
public:
	AllOutGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Mass Units", 6, sc2::UNIT_TYPEID::PROTOSS_STALKER, 10, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 2, sc2::UNIT_TYPEID::PROTOSS_VOIDRAY, 5));
		this->BaseAction::name = "All In";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return obs->GetArmyCount() > 1;
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

		return madeStaker;
	}
};