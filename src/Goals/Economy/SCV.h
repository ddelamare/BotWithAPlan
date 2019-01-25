#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Constants.h"
class SCVGoal : public BaseAction
{
public:
	SCVGoal() : BaseAction() {
		name = "Build SCV";
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_SCV, 1));

	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		units.clear();
		double score = 0.0f;
		auto workers = obs->GetUnits(IsWorker());
		if (workers.size() >= Constants::HARD_WORKER_CAP) return 0; // Cap max number of workers/active bases
		auto townHall = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		for (auto th : townHall)
		{
			auto needed = th->ideal_harvesters - th->assigned_harvesters;
			score += needed;
			units.push_back(th);	
		}
		auto gasHaverstBuilding = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
		for (auto ghb : gasHaverstBuilding)
		{
			auto needed = ghb->ideal_harvesters - ghb->assigned_harvesters;
			score += needed;
			units.push_back(ghb);
		}
		score -= obs->GetIdleWorkerCount();

		// If we are below ideal harvesters, give it more weight
		if (score > 0)
		{
			score += 5; 
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool builtProbe = false;
		for (auto th : units) {
			if (th->orders.size() == 0)
			{
				actions->UnitCommand(th, ABILITY_ID::TRAIN_SCV);
				builtProbe = true;
			}
		}

		return builtProbe;
	}


};