#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class ScoutSweepGoal : public BaseAction
{
	int rotation = 0;
public:
	ScoutSweepGoal() : BaseAction() {
		this->BaseAction::name = "Scout Sweep";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		if (state->ScoutingUnits.size()) return 0;
		return 1 + ( obs->GetIdleWorkerCount() > 1);
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool queuedScouting = false;
		auto idleWorkers = obs->GetUnits(sc2::Unit::Alliance::Self, IsIdleWorker());
		if (idleWorkers.size())
		{
			// Send it to every expo
			for (int i = state->ExpansionLocations.size() - 1; i >= 0; i--)
			{
				actions->UnitCommand(idleWorkers[0], ABILITY_ID::SMART, state->ExpansionLocations[(i + rotation) % state->ExpansionLocations.size()], true);
				state->ScoutingUnits.push_back(idleWorkers[0]);
			}
			rotation++;
		}

		return queuedScouting;
	}
};