#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
#include "Common/Strategy/Building/SpiralStrategy.h"
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
			if (building->orders.size() == 0)
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
			if (building->orders.size() == 0)
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
		bool madeUnit = false;
		auto buildings = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_WARPGATE));
		UnitTypes unitTypes = obs->GetUnitTypeData();
		UnitTypeData unittype = unitTypes[(int)unit_type];
		auto warpAbility = (ABILITY_ID) ((int)unittype.ability_id + WARPCONVERSION_DIFFERENCE);
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
				SpiralStrategy strat(warpAbility, false, false, .5);
				actions->UnitCommand(building, warpAbility, strat.FindPlacement(obs,actions,query,debug, state));
				madeUnit = true;
			}
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

		auto worker = FindClosestAvailableWorker(buildPos, obs, query, state);
		if (DistanceSquared3D(buildPos, Point3D()) > 0)
		{
			// For vespene collectors, build ability must target geyser
			if (buildingStrategy->foundUnit)
			{
				actions->UnitCommand(worker, build_ability, buildingStrategy->foundUnit);
			}
			// Standard Placement
			else
			{
				actions->UnitCommand(worker, build_ability, buildPos);
			}
			debug->DebugSphereOut(buildPos, 3, Colors::Purple);
			return true;
		}

		return false;
	}

	bool static TryBuildBuilding(AbilityID build_ability, UNIT_TYPEID unitType, const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return TryBuildBuilding(build_ability, unitType, obs, actions, query, debug,  state, new SpiralStrategy(build_ability, true, true));
	}

}