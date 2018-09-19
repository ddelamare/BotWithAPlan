#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class AttackProxyGoal : public BaseAction
{
public:
	AttackProxyGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->BaseAction::name = "Attack Proxy";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto enemyBuildings = Util::FindNearbyUnits(IsEnemyBuilding(), Util::ToPoint3D(state->StartingLocation), obs, 40);
		if (enemyBuildings.size())
		{
			return 10;
		}
		else
			return 0;	
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool attacked = false;
		auto enemyBuildings = Util::FindNearbyUnits(IsEnemyBuilding(), Util::ToPoint3D(state->StartingLocation), obs, 40);
		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		auto targetPoint = VectorHelpers::GetAveragePoint(enemyBuildings);
		actions->UnitCommand(army, ABILITY_ID::ATTACK, targetPoint);
		return false;
	}
};