#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"
#include "../../Common/UnitFilters.h"
#include "../../Planner/Actions/UseAbility.h"

class ChronoGoal : public BaseGoal
{
public:
	double virtual CalculateScore(const sc2::ObservationInterface *obs)
	{
		units.clear();
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		double score = 15.0f;

		// Find nexuses that can cast chrono
		for (auto nexus : nexuses)
		{
			if (nexus->energy > 50)
			{
				units.push_back(nexus);
			}
		}

		// Find targets
		auto buildings = obs->GetUnits(Unit::Alliance::Self, IsBuilding());
		for (auto building : buildings)
		{
			if (building->orders.size() > 0)
			{
				//Check for buildings that are building something
				auto currentOrder = building->orders[0];
				// TODO: no double buff
				if (currentOrder.progress > 0)
				{
					units.push_back(building);
				}
			}
		}
		// If we have the right number of units, return the score.
		if (units.size() > 0 && units.size() % 2 == 0)
			return score;
		else
			return 0;

	};
	BaseAction virtual *GetAction()
	{
		return new UseAbility("Chrono Boost");
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query)
	{
		bool didChrono = false;
		for (int i = 0; i < units.size(); i += 2)
		{
			auto caster = units[i];
			auto target = units[i + 1];
			actions->UnitCommand(caster, CHRONO_OVERRIDE, target);
		}
		return didChrono;
	}
};