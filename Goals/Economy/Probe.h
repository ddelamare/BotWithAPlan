#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"
class ProbeGoal : public BaseGoal
{
public:
	ProbeGoal() : BaseGoal() {
		goal = new BuildResource(RESOURCES::PROBE);
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		units.clear();
		double score = 0.0f;
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		for (auto nexus : nexuses)
		{
			auto needed = nexus->ideal_harvesters - nexus->assigned_harvesters;
			score += needed;
			units.push_back(nexus);	
		}
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, GetUnitsOfType(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
		for (auto assim : assimilators)
		{
			auto needed = assim->ideal_harvesters - assim->assigned_harvesters;
			score += needed;
			units.push_back(assim);
		}
		score -= obs->GetIdleWorkerCount();

		// If we are below ideal harvesters, give it more weight
		if (score > 0)
		{
			score += 10; 
		}
		return score;
	};
	BaseAction virtual *GetAction() {
		return goal;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query) 
	{
		bool builtProbe = false;
		for (auto nexus : units) {			if (nexus->orders.size() == 0)
			{
				actions->UnitCommand(nexus, ABILITY_ID::TRAIN_PROBE);
				builtProbe = true;
			}		}		return builtProbe;	}

};