#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class AttackProxyGoal : public BaseAction
{
	bool spokenAlready = false;
public:
	AttackProxyGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->conditions.push_back(new BaseCondition("Static Defenses",4, UNIT_TYPEID::PROTOSS_PHOTONCANNON, 3, UNIT_TYPEID::PROTOSS_SHIELDBATTERY,1));
		this->BaseAction::name = "Attack Proxy";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto enemyBuildings = Util::FindNearbyUnits(IsEnemyBuilding(), Util::ToPoint3D(state->StartingLocation), obs, 60);
		if (enemyBuildings.size())
		{

			return 10;
		}
		else
			return 0;	
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		if (!spokenAlready)
		{
			actions->SendChat("I SEE YOU THERE!!!");
			spokenAlready = true;
		}
		bool attacked = false;
		auto enemyBuildings = Util::FindNearbyUnits(IsEnemyBuilding(), Util::ToPoint3D(state->StartingLocation), obs, 40);
		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		auto targetPoint = Util::GetAveragePoint(enemyBuildings);

		state->ArmyManager->RequestAction(targetPoint, BattleMode::Attack);

		return false;
	}
};