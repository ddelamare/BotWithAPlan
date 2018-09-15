#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"

class RushGoal : public BaseAction
{
public:
	RushGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Mass Units", 4, sc2::UNIT_TYPEID::PROTOSS_VOIDRAY, 4, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 2));
		this->BaseAction::name = "Rush";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 5;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		Util::TryBuildUnit(ABILITY_ID::TRAIN_VOIDRAY, UNIT_TYPEID::PROTOSS_STARGATE, obs, actions);

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
		actions->UnitCommand(army, ABILITY_ID::ATTACK, attackPoint);

		return madeStaker;
	}
};