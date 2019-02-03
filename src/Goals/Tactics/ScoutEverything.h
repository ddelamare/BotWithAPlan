#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class ScoutEverythingGoal : public BaseAction
{

public:
	ScoutEverythingGoal() : BaseAction() {
		this->BaseAction::name = "Scout Sweep";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {

		auto score = 0.0;
		// If we can see the main base and there's nothing there, and no other enemies are visible
		// we need to find the last ones by scouting everywhere
		if (obs->GetVisibility(state->EnemyBase) == Visibility::Visible)
		{
			auto enemies = obs->GetUnits(Unit::Alliance::Enemy);
			if (enemies.size() == 0)
			{
				// Good chance we have killed everything at main
				score = 7;
			}
		}

		return score;

	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool queuedScouting = false;
		auto units = obs->GetUnits(sc2::Unit::Alliance::Self);
		if (units.size())
		{
			// Send it to every expo
			for (int i = 0; i < units.size(); i++)
			{
				actions->UnitCommand(units[i], ABILITY_ID::SMART, state->ExpansionLocations[i % state->ExpansionLocations.size()], true);
			}
			queuedScouting = true;
		}
		
		return queuedScouting;
	}
};