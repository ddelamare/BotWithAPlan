#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class CannonDefenseGoal : public BaseAction
{
	BaseCondition* cannons;
public:
	CannonDefenseGoal() : BaseAction() {
		cannons = new BaseCondition("Static Defenses", 4, UNIT_TYPEID::PROTOSS_PHOTONCANNON, 7, UNIT_TYPEID::PROTOSS_SHIELDBATTERY, 3);
		this->conditions.push_back(cannons);
		this->BaseAction::name = "Build me some cannons";
		holdOtherGoals = true;
	}

	//TODO: if rush is detected
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		if (!cannons->IsMet(state))
		{
			return 20;
		}
		else
			return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return false;
	}
};