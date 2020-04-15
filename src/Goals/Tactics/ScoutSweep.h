#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class ScoutSweepGoal : public BaseAction
{
	int rotation = 0;
	long timeSinceLastExec = 0;
public:
	ScoutSweepGoal() : BaseAction() {
		this->BaseAction::name = "Scout Sweep";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		timeSinceLastExec++;
		if (state->ScoutingUnits.size() || state->KilledScouts > 3)  return 0;
		return 1 + ( obs->GetIdleWorkerCount() > 1) + (timeSinceLastExec / 100.0);
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool queuedScouting = false;
		auto idleWorkers = obs->GetUnits(sc2::Unit::Alliance::Self, IsIdleWorker());

		if (idleWorkers.size() && !state->ScoutingUnits.size() && state->KilledScouts < 3)
		{
			auto start = state->ExpansionLocations[(rotation) % state->ExpansionLocations.size()];
			auto shortestPath = Util::FindShortestPath(start, state->ExpansionLocations);
			// Send it to every expo
			for (int i = shortestPath.size() - 1; i >= 0; i--)
			{
				actions->UnitCommand(idleWorkers[0], ABILITY_ID::SMART, shortestPath[i], true);
			}
			state->ScoutingUnits.push_back(idleWorkers[0]);
			rotation++;
		}
		timeSinceLastExec = 0;


		return queuedScouting;
	}
};