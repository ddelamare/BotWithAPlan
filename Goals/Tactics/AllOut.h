#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>

class AllOutGoal : public BaseAction, public BaseCondition
{
public:
	AllOutGoal() : BaseAction(), BaseCondition() {
		this->conditions.push_back(new BaseCondition("Mass Units", 4, sc2::UNIT_TYPEID::PROTOSS_STALKER, 10, sc2::UNIT_TYPEID::PROTOSS_VOIDRAY,5));
		this->BaseAction::name = "Build Stalker";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto stalkers = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_STALKER));
		return stalkers.size() > 5;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto army = obs->GetUnits(Unit::Alliance::Self, IsCombatUnit());
		actions->UnitCommand(army, ABILITY_ID::ATTACK, state->EnemyBase);

		return madeStaker;
	}
};