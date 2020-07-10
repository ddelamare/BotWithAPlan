#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Conditions/HaveBigArmy.h"		 
#include "sc2api\sc2_api.h"
#include "Planner/Actions/BuildResource.h"
#include "Common/Resource.h"
#include "Common/Util/Util.h"

class ForgeFastExpandGoal : public BaseAction
{
public:
	ForgeFastExpandGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Forge Nexus", 4, UNIT_TYPEID::PROTOSS_FORGE, 1, UNIT_TYPEID::PROTOSS_NEXUS, 2));
		this->BaseAction::name = "Forge Fast Expand";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		if (state->opponentRace == Race::Zerg)
		{
			if (state->CurrentUnits[sc2::UNIT_TYPEID::PROTOSS_NEXUS] == 1 && obs->GetGameLoop() < 4000)
				return 5;
		}
		else
			return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		// Nothing to do here
		return false;
	}
};