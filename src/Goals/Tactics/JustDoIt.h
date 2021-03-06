#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"

class JustDoitGoal : public BaseAction
{
	ThreatAnalyzer threatAnalyzer;
public:
	JustDoitGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->BaseAction::name = "Just Do It";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {

		if (obs->GetFoodUsed() > 180 || obs->GetUnits(Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE)).size() > 8)
			return 5;

		return (obs->GetFoodArmy() / 75) * threatAnalyzer.GetThreat(&state->threat);
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		auto visibleBuildings = obs->GetUnits(Unit::Alliance::Enemy, IsVisibleArmy());
		auto enemyStuff = obs->GetUnits(Unit::Alliance::Enemy, IsNotFiller());
		auto attackPoint = state->EnemyBase;
		if (visibleBuildings.size())
		{
			attackPoint = visibleBuildings[0]->pos;
		}
		else if (enemyStuff.size())
		{
			std::sort(enemyStuff.begin(), enemyStuff.end(), Sorters::sort_by_distance(Util::GetAveragePoint(army)));
			
			attackPoint = enemyStuff[0]->pos;
		}

		state->ArmyManager->RequestAction(attackPoint, BattleMode::Attack);


		return madeStaker;
	}
};