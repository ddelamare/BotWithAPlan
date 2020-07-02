#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util.h"

class ScoutMainGoal : public BaseAction
{
public:
	ScoutMainGoal() : BaseAction() {
		this->BaseAction::name = "Scout Main";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		if (state->ScoutingUnits.size() > 1) return 0;
		auto loop = obs->GetGameLoop();
		if (loop > 800 && loop < 2000 && state->KilledScouts < 2)
			return 2 + (obs->GetIdleWorkerCount() > 1);
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool queuedScouting = false;
		auto idleWorkers = obs->GetUnits(sc2::Unit::Alliance::Self, IsIdleWorker());
		if (idleWorkers.size() && state->ScoutingUnits.size() <= 1)
		{
			actions->UnitCommand(idleWorkers[0], ABILITY_ID::SMART, state->EnemyBase, true);
			state->ScoutingUnits.push_back(idleWorkers[0]);
		}

		return queuedScouting;
	}
};