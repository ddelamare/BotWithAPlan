#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class DefendWithUnitsGoal : public BaseAction
{
public:
	DefendWithUnitsGoal() : BaseAction() {
		this->conditions.push_back(new HaveBigArmy(50));
		this->BaseAction::name = "Defend with Workers";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto enemyUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy,IsEnemy(), Util::ToPoint3D(state->StartingLocation), obs, 20);
		if (enemyUnits.size())
		{
			if (enemyUnits.size() > obs->GetFoodArmy())
				return 10;
		}
			return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool attacked = false;
		auto enemyUnits = Util::FindNearbyUnits(IsEnemy(), Util::ToPoint3D(state->StartingLocation), obs, 40);
		auto army = obs->GetUnits(Unit::Alliance::Self);
		auto targetPoint = VectorHelpers::GetAveragePoint(enemyUnits);
		actions->UnitCommand(army, ABILITY_ID::ATTACK, targetPoint);
		return false;
	}
};