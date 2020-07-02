#pragma once
#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util.h"
#include "Common/Analyzers/RushAnalyzer.h"

class RushDefenseGoal : public BaseAction
{
	RushAnalyzer rushAnalyzer;
public:
	RushDefenseGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Stuff", 4, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 3, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		holdOtherGoals = true;
		name = "Rush Defense";
	}

	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		return 0;
		if (this->conditions[0]->IsMet(state))
			
		return rushAnalyzer.GetRushPossibiliy(obs);
	};

	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return false;
	}

	bool virtual HoldOtherGoals(const sc2::ObservationInterface* obs) { 
		return holdOtherGoals; 
	}

};