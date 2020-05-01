#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
#include "Common/Strategy/Building/SpiralStrategy.h"
#include "Common/Strategy/Building/GridStrategy.h"
namespace Util {

	static Point3D GetAveragePoint(Units units)
	{
		auto vectorToCenter = Point3D();

		for (auto u : units)
		{
			vectorToCenter += u->pos;
		}
		// Get average enemy location
		return vectorToCenter / units.size();
	}

	bool static TryBuildUnit(AbilityID train_unit_ability, UnitTypeID buildingType, const ObservationInterface* obs, sc2::ActionInterface* actions)
	{
		bool madeUnit = false;
		auto buildings = obs->GetUnits(Unit::Alliance::Self, IsUnit(buildingType));
		for (auto building : buildings) {
			if (building->build_progress < 1.0)
			{
				continue;
			}
			if (building->orders.size() == 0)
			{
				actions->UnitCommand(building, train_unit_ability);
				madeUnit = true;
			}
			else if (building->add_on_tag && IsReactor()(*obs->GetUnit(building->add_on_tag)) && building->orders.size() < 2)
			{
				actions->UnitCommand(building, train_unit_ability);
				madeUnit = true;
			}
		}

		return madeUnit;
	}

	bool static TryBuildUnit(sc2::UNIT_TYPEID unit_type, UnitTypeID buildingType, const ObservationInterface* obs, sc2::ActionInterface* actions, QueryInterface* query)
	{
		bool madeUnit = false;
		UnitTypes unitTypes = obs->GetUnitTypeData();
		UnitTypeData unittype = unitTypes[(int)unit_type];
		auto buildings = obs->GetUnits(Unit::Alliance::Self, IsUnit(buildingType));
		for (auto building : buildings) {
			if (building->orders.size() == 0 && building->build_progress >= 1.0)
			{
				actions->UnitCommand(building, unittype.ability_id);
				madeUnit = true;
			}
		}

		return madeUnit;
	}

	bool static TryWarpUnit(sc2::UNIT_TYPEID unit_type, const ObservationInterface* obs, sc2::ActionInterface* actions, QueryInterface* query, DebugInterface* debug, GameState* state)
	{
		// All the abilities for the warp version of train are exactly 497 from the ability of the regular train
		const int WARPCONVERSION_DIFFERENCE = 497;
		int madeUnit = 0;
		auto buildings = obs->GetUnits(Unit::Alliance::Self, CompletedUnits(UNIT_TYPEID::PROTOSS_WARPGATE));
		auto unitTypes = &state->UnitInfo;
		UnitTypeData unittype = (*unitTypes)[(int)unit_type];
		auto warpAbility = (ABILITY_ID) ((int)unittype.ability_id + WARPCONVERSION_DIFFERENCE);
		int MAX_BUILD_PER_FRAME = 2;
		for (auto building : buildings) {
			auto abilities = query->GetAbilitiesForUnit(building).abilities;

			bool hasability = false;
			for (auto abi : abilities)
			{
				if (abi.ability_id == warpAbility)
				{
					hasability = true;
					break;
				}
			}

			if (hasability)
			{
				GridStrategy strat(warpAbility, false, false, 1);
				actions->UnitCommand(building, warpAbility, strat.FindPlacement(obs,actions,query,debug, state));
				madeUnit++;
			}

			if (madeUnit >= MAX_BUILD_PER_FRAME)
				break;
		}
		if (!madeUnit)
			return TryBuildUnit(unit_type, UNIT_TYPEID::PROTOSS_GATEWAY, obs, actions, query);

		return madeUnit;
	}

	bool static TryBuildBuilding(AbilityID build_ability, UNIT_TYPEID unitType, const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state, BuildingStrategy* buildingStrategy)
	{
		bool success = false;

		// Is a worker already on the way?
		bool alreadyBuilding = IsAnyWorkerCommitted(build_ability, obs);
		auto buildingData = obs->GetUnitTypeData()[(int)unitType];
		if (alreadyBuilding || !HasEnoughResources(buildingData.mineral_cost, buildingData.vespene_cost, obs)) return false;

		Point3D buildPos = buildingStrategy->FindPlacement(obs, actions, query, debug, state);

		auto worker = FindClosestAvailableBuilder(buildPos, obs, query, state);
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			// For vespene collectors, build ability must target geyser
			if (buildingStrategy->foundUnit)
			{
				actions->UnitCommand(worker, build_ability, buildingStrategy->foundUnit);
				actions->UnitCommand(worker, ABILITY_ID::STOP, true); // Send worker to idle pool to be reassigned
			}
			// Standard Placement
			else if (worker)
			{
				actions->UnitCommand(worker, build_ability, buildPos, IsBuildingStructure()(*worker, state));
			}
			debug->DebugSphereOut(buildPos, 3, Colors::Purple);
			return true;
		}

		return false;
	}

	bool static TryBuildBuilding(AbilityID build_ability, UNIT_TYPEID unitType, const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return TryBuildBuilding(build_ability, unitType, obs, actions, query, debug,  state, new SpiralStrategy(build_ability, true, true, state->selfRace));
	}

	// Taken from sc2api/sc2libs/sc2_search and modfied with unit sizes
	std::vector<std::pair<Point3D, Units>> static FindClusters(const Units& units, float distance_apart)
	{
		float squared_distance_apart = distance_apart * distance_apart;
		std::vector<std::pair<Point3D, Units>> clusters;
		for (size_t i = 0, e = units.size(); i < e; ++i) {
			auto u = units[i];

			float distance = std::numeric_limits<float>::max();
			std::pair<Point3D, Units>* target_cluster = nullptr;
			// Find the cluster this unit is closest to.
			for (auto& cluster : clusters) {
				float d = DistanceSquared3D(u->pos, cluster.first);
				if (d < distance) {
					distance = d;
					target_cluster = &cluster;
				}
			}

			// If the target cluster is some distance away don't use it.
			if (distance > squared_distance_apart) {
				clusters.push_back(std::make_pair(u->pos, Units{u}));
				continue;
			}

			if (target_cluster == nullptr)
			{
				continue;
			}

			// Otherwise append to that cluster and update it's center of mass.
			target_cluster->second.push_back(u);
			size_t size = target_cluster->second.size();
			target_cluster->first = ((target_cluster->first * (float(size) - 1)) + u->pos) / float(size);
		}

		return clusters;
	}

	int static GetUnitValues(Units units, UnitTypes info)
	{
		int sum = 0;
		for (auto unit : units)
		{
			sum += info[unit->unit_type].mineral_cost + info[unit->unit_type].vespene_cost;
		}

		return sum;
	}

}