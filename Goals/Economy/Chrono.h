#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Common/UnitFilters.h>
#include <Planner/Actions/UseAbility.h>
#include <algorithm>    // std::remove_if

// There's this random buff that shows up and keeps ticking up.
bool IsNotUnknownBuff(sc2::BUFF_ID i) { return (i != (BUFF_ID)281); }

class ChronoGoal : public BaseAction
{
private:
public:
	ChronoGoal() : BaseAction() {
		name = "Use Chrono";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state)
	{
		units.clear();
		targets.clear();
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		double score = 15.0f;

		// Find nexuses that can cast chrono		  
		for (auto nexus : nexuses)
		{
			if (nexus->energy > 50 && nexus->build_progress >= 1)
			{
				units.push_back(nexus);
			}
		}

		// Find targets
		auto buildings = obs->GetUnits(Unit::Alliance::Self, IsBuilding());
		for (auto building : buildings)
		{
			//TODO: Check for specific chrono buff.
			auto foundBuff = find_if(building->buffs.begin(),
				building->buffs.end(),
				IsNotUnknownBuff
			);
			if (building->orders.size() > 0 && foundBuff == building->buffs.end())
			{
				//Check for buildings that are building something
				auto currentOrder = building->orders[0];
				if (currentOrder.progress > 0)
				{
					targets.push_back(building);					  
				}
			}
		}
		// If we have the right number of units, return the score.
		if (units.size() > 0 && targets.size() > 0)
			return score;
		else
			return 0;

	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool didChrono = false;
		for (int i = 0; i < units.size() && i < targets.size(); i++)
		{
			auto caster = units[i];
			auto target = targets[i];
			actions->UnitCommand(caster, CHRONO_OVERRIDE, target);
			didChrono = true;
		}
		return didChrono;
	}
};