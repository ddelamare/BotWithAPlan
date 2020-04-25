#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/UnitFilters.h"

class OracleHarrassGoal : public BaseAction
{
public:
	OracleHarrassGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Oracle Harass", 2, sc2::UNIT_TYPEID::PROTOSS_ORACLE, 2));
		this->BaseAction::name = "Oracle Harass";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		if (obs->GetGameLoop() < 6000 || obs->GetGameLoop() > 9000 || true) return 0;
		return 5;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto visibleBuildings = obs->GetUnits(Unit::Alliance::Enemy, IsTownHall());
		auto attackPoint = state->EnemyBase;
		if (visibleBuildings.size())
		{
			attackPoint = visibleBuildings[0]->pos;
		}

		state->ArmyManager->RequestAction(attackPoint, BattleMode::Harrass);

		return madeStaker;
	}
};