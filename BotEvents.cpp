#include "BotWithAPlan.h"
#include "Common\UnitFilters.h"
#include "Common\Util.h"
void BotWithAPlan::OnBuildingConstructionComplete(const Unit*)
{
	shouldRecalcuate = true;
}

void::BotWithAPlan::OnUnitCreated(const Unit* unit)							   
{
	if (unit->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
	{
		// Clear Rally Point
		Actions()->UnitCommand(unit, ABILITY_ID::RALLY_WORKERS, unit->pos);
	}
	state.CurrentUnits[unit->unit_type]++;
}

//! Called whenever one of the player's units has been destroyed.
//!< \param unit The destroyed unit.
void BotWithAPlan::OnUnitDestroyed(const Unit* unit) {
	state.CurrentUnits[unit->unit_type]--;
	if (VectorHelpers::FoundInVector(state.ScoutingUnits, unit))
	{
		VectorHelpers::RemoveFromVector(&state.ScoutingUnits, unit);
	}
}


void BotWithAPlan::OnUnitEnterVision(const Unit* unit) 
{
	state.ObservedUnits[unit->unit_type] = 1;
	auto nexuses = Observation()->GetUnits(Unit::Alliance::Self, IsTownHall());
	for (auto th : nexuses)
	{
		if (Distance3D(unit->pos, th->pos) < 25)
		{
			// RALLY THE TROOPS!
			auto army = Observation()->GetUnits(Unit::Alliance::Self, IsCombatUnit());
			Actions()->UnitCommand(army, ABILITY_ID::ATTACK, unit->pos);
		}
	}
}

void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
	auto nexus = Observation()->GetUnits(IsTownHall())[0];
	Actions()->UnitCommand(nexus, ABILITY_ID::SMART, nexus->pos);
	auto enemyLocations = Observation()->GetGameInfo().enemy_start_locations;
	state.EnemyBase = enemyLocations[0];

	// Get all minerals and sort by x , then y pos
	auto minerals = Observation()->GetUnits(Unit::Alliance::Neutral, IsMineralField());
	int count = 0;
	Point3D sum = Point3D();
	const int MINERAL_DISTANCE_THRESHOLD = 150;
	while (minerals.size() > 0)
	{
		Point3D origMineral = minerals[0]->pos;
		sum = Point3D();
		count = 0;
		for (int j = 0; j < minerals.size();)
		{
			auto cluster = std::vector<Point3D>();
			auto dis = DistanceSquared3D(origMineral, minerals[j]->pos);
			if (dis < MINERAL_DISTANCE_THRESHOLD)
			{
				sum += minerals[j]->pos;
				count++;
				// Erase this element
				minerals.erase(minerals.begin() + j, minerals.begin() + j + 1);
			}
			else
			{
				j++;
			}
		}

		state.ExpansionLocations.push_back(sum / count);
	}
	// Remove starting pos as expansion location
	auto closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, nexus->pos);
	state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));
	closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, Point3D(state.EnemyBase.x, state.EnemyBase.y, 0));
	state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));
    
	// Sort expansions by distance to home base
	std::sort(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), Sorters::sort_by_distance(nexus->pos));
}

