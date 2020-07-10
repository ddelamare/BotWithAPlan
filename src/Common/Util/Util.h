#pragma once
#include "sc2api\sc2_api.h"
#include "sc2api/sc2_unit_filters.h"
#include "UnitFilters.h"
#include "CombatUtil.h"
#include "VectorHelpers.h"
#include <vector>
using namespace sc2;

namespace Util
{

	// Basically this function outputs a lower value as current percent increases over hallNeedingMiners percent and when percent is 0, returns score when zero
	const static double FeedbackFunction(double currentPercent, double targetPercent, double multiplier)
	{
		// equivalent to 1/((currentPercent / targetPercent) + (1/multiplier));
		return targetPercent / (currentPercent + (targetPercent / multiplier));
	}

	const static double ExponentialIncrease(double x, double dampener)
	{
		return x * x * dampener;
	}

	const static Unit* FindNearestResourceNeedingHarversters(const Unit* worker, const ObservationInterface* obs, QueryInterface* query, bool prioritizeGas)
	{
		double distance = DBL_MAX;
		const Unit* hallNeedingMiners = 0;
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		for (auto nexus : nexuses)
		{
			if (nexus->ideal_harvesters > nexus->assigned_harvesters)
			{
				auto dis = query->PathingDistance(worker->pos, nexus->pos + Point3D(nexus->radius, 0, 0));
				if (dis < distance)
				{
					distance = dis;
					hallNeedingMiners = nexus;
				}
			}

		}

		if (hallNeedingMiners)
		{
			auto nex = hallNeedingMiners;
			// Find nearby mineral patch.
			Units minerals = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField());
			float distance = std::numeric_limits<float>::max();
			for (const auto& m : minerals) {
				float d = DistanceSquared2D(m->pos, nex->pos);
				if (d < distance) {
					distance = d;
					hallNeedingMiners = m;
				}
			}
		}
		// Only assign gas after minerals have been filled
		if (!hallNeedingMiners || prioritizeGas)
		{
			distance = DBL_MAX;
			auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsGasBuilding());
			for (auto assim : assimilators)
			{
				if (assim->ideal_harvesters > assim->assigned_harvesters)
				{
					auto dis = query->PathingDistance(worker->pos, assim->pos + Point3D(assim->radius, 0, 0));
					if (dis < distance)
					{
						distance = dis;
						hallNeedingMiners = assim;
					}
				}
			}
		}

		return hallNeedingMiners;
	}

	const static Unit* FindClosestAvailableBuilder(Point3D point, const ObservationInterface* obs, QueryInterface* query, GameState* state, bool includeUnpathable)
	{
		auto units = obs->GetUnits(Unit::Alliance::Self, IsWorker());
		double distance = DBL_MAX;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			auto dis = query->PathingDistance(unit, point);
			// Only count distances that are greater than 0. The exception is for gas geysers, which are not pathable. Hence the flag.
			if ((dis > 0.1f || includeUnpathable) && dis < distance && !VectorHelpers::FoundInVector(state->ScoutingUnits, unit))
			{
				distance = dis;
				foundUnit = unit;
			}
		}
		return foundUnit;
	}

	const static Unit* FindClosetOfType(Units units, Point3D point, const ObservationInterface* obs, QueryInterface* query, bool usePathing)
	{
		double distance = DBL_MAX;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			double dis = DBL_MAX;
			if (usePathing)
			{
				dis = query->PathingDistance(unit, point);
			}
			else
			{
				dis = DistanceSquared2D(unit->pos, point);
			}
			if (dis < distance)
			{
				distance = dis;
				foundUnit = unit;
			}
		}
		return foundUnit;
	}

	const static Point3D FindClosestPoint(std::vector<Point3D> points, Point3D point)
	{
		auto distance = DBL_MAX;
		Point3D closest;
		for (auto p : points)
		{
			auto dis = DistanceSquared3D(p, point);
			if (dis < distance)
			{
				distance = dis;
				closest = p;
			}
		}
		return closest;
	}

	const static Unit* FindFurthestInRadius(Filter filter, Point3D point, const ObservationInterface* obs, QueryInterface* query, double radius, Point3D ignorePoint)
	{
		auto units = obs->GetUnits(filter);
		double distance = DBL_MIN;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			if (unit->pos.x == ignorePoint.x && unit->pos.y == ignorePoint.y) continue;
			if (Distance3D(ignorePoint, unit->pos) < 3) continue;
			auto dis = Distance3D(unit->pos, point);
			if (dis > distance && dis < radius)
			{
				distance = dis;
				foundUnit = unit;
			}
		}
		return foundUnit;
	}

	const static Units FindNearbyUnits(Units* units, Point3D point, const ObservationInterface* obs, double radius)
	{
		auto unitsNearby = Units();
		for (auto unit : *units)
		{
			auto dis = Distance3D(unit->pos, point);
			if (dis < radius)
			{
				unitsNearby.push_back(unit);
			}
		}
		return unitsNearby;
	}


	const static Units FindNearbyUnits(sc2::Unit::Alliance alliance, Filter filter, Point3D point, const ObservationInterface* obs, double radius)
	{
		auto units = obs->GetUnits(alliance, filter);
		return Util::FindNearbyUnits(&units, point, obs, radius);
	}

	bool static DoesAnyUnitHaveOrder(Units units, ABILITY_ID ability)
	{
		for (auto unit : units)
		{
			if (unit->orders.size())
			{
				for (auto order : unit->orders)
				{
					if (order.ability_id == ability)
						return true;
				}
			}
		}
		return false;
	}

	Units static GetUnitsWithOrder(Units units, ABILITY_ID ability)
	{
		Units unitsWithAbility;
		for (auto unit : units)
		{
			if (unit->orders.size())
			{
				for (auto order : unit->orders)
				{
					if (order.ability_id == ability)
						unitsWithAbility.push_back(unit);
				}
			}
		}
		return unitsWithAbility;
	}


	bool static DoesAnyUnitHaveOrder(Filter filter, ABILITY_ID ability, const ObservationInterface* obs)
	{
		auto units = obs->GetUnits(Unit::Alliance::Self, filter);
		return DoesAnyUnitHaveOrder(units, ability);
	}

	bool static IsAnyWorkerCommitted(ABILITY_ID ability, const ObservationInterface* obs)
	{
		return DoesAnyUnitHaveOrder(IsWorker(), ability, obs);
	}


	bool static HasEnoughResources(int mineralsNeeded, int gasNeeded, const ObservationInterface* obs)
	{
		return obs->GetMinerals() >= mineralsNeeded && obs->GetVespene() >= gasNeeded;
	}

	Point3D static ToPoint3D(Point2D point)
	{
		return Point3D(point.x, point.y, 0);
	}

	Point3D static ToPoint3D(Point2D point, float z)
	{
		return Point3D(point.x, point.y, z);
	}

	std::string static GetStringFromRace(const sc2::Race& race)
	{
		switch (race)
		{
		case sc2::Race::Protoss: return "Protoss";
		case sc2::Race::Terran:  return "Terran";
		case sc2::Race::Zerg:	return "Zerg";
		default: return "Random";
		}
	}

	// This is a naive implementation since traveling salesman in npHard 
	std::vector<Point3D> static FindShortestPath(Point3D startingPoint, std::vector<Point3D> pointsToVisit)
	{
		auto alreadyVisited = std::vector<Point3D>();
		alreadyVisited.push_back(startingPoint);
		auto currentPoint = &startingPoint;
		for (int i = 0; i < pointsToVisit.size(); i++)
		{
			if (VectorHelpers::FoundInVector(alreadyVisited, pointsToVisit[i]) || &pointsToVisit[i] == currentPoint)
			{
				continue;
			}
			double min = DBL_MAX;
			Point3D* min_point = 0;
			for (int j = 0; j < pointsToVisit.size(); j++)
			{
				if (VectorHelpers::FoundInVector(alreadyVisited, pointsToVisit[j]) || &pointsToVisit[j] == currentPoint)
				{
					continue;
				}
				auto dis = DistanceSquared3D(*currentPoint, pointsToVisit[j]);
				if (dis < min)
				{
					min = dis;
					min_point = &pointsToVisit[j];
				}
			}
			currentPoint = min_point;
			alreadyVisited.push_back(*min_point);
		}

		return alreadyVisited;
	}

	int static GetUnitValues(Units units, UnitTypes* info)
	{
		int sum = 0;
		for (auto unit : units)
		{
			sum += (*info)[unit->unit_type].mineral_cost + (*info)[unit->unit_type].vespene_cost;
		}

		return sum;
	}


	double static GetUnitPercent(Filter filter, int foodCost, const ObservationInterface* obs)
	{
		int unitFood = foodCost * obs->GetUnits(sc2::Unit::Alliance::Self, filter).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent of filtered unit
		return percent;
	}

	double static GetUnitPercent(UNIT_TYPEID unitType, int foodCost, const ObservationInterface* obs)
	{
		return GetUnitPercent(IsUnit(unitType), foodCost, obs);
	}

	bool static IsPossibleToPathTo(const sc2::ObservationInterface* obs, const sc2::Point3D& expandLocation, sc2::QueryInterface* query)
	{

		auto randoWorker = obs->GetUnits(sc2::Unit::Alliance::Self, IsWorker());

		std::vector<QueryInterface::PathingQuery> queries;
		for (int i = 0; i < randoWorker.size() && i < 5; i++)
		{
			QueryInterface::PathingQuery pq;
			pq.start_unit_tag_ = randoWorker[i]->tag;
			pq.start_ = randoWorker[i]->pos;
			pq.end_ = expandLocation;

			queries.push_back(pq);
		}
		auto res = query->PathingDistance(queries);

		bool canPath = false;
		for (int i = 0; i < res.size(); i++)
		{
			if (res[i] > 0.1f)
			{
				canPath = true;
				break;
			}
		}

		return canPath;
	}

};

namespace Sorters
{
	struct sort_by_x {
		bool operator()(Point3D const& lhs, Point3D const& rhs)
		{
			return lhs.x < rhs.x;
		}
		bool operator()(const Unit* lhs, const Unit* rhs)
		{
			return lhs->pos.x < rhs->pos.x;
		}
	};
	struct sort_by_y {
		bool operator()(Point3D const& lhs, Point3D const& rhs)
		{
			return lhs.y < rhs.y;
		}
		bool operator()(const Unit* lhs, const Unit* rhs)
		{
			return lhs->pos.y < rhs->pos.y;
		}
	};
	struct sort_by_distance {
		Point2D referencePoint;
		sort_by_distance(Point3D point) {
			referencePoint = point;
		}
		sort_by_distance(Point2D point) {
			referencePoint = point;
		}

		bool operator()(Point3D const& lhs, Point3D const& rhs)
		{
			return Distance2D(lhs, referencePoint) < Distance2D(rhs, referencePoint);
		}

		bool operator()(Unit const* lhs, Unit const* rhs)
		{
			return Distance2D(lhs->pos, referencePoint) < Distance2D(rhs->pos, referencePoint);
		}

		bool operator()(QueryInterface::PlacementQuery const lhs, QueryInterface::PlacementQuery const rhs)
		{
			return Distance2D(lhs.target_pos, referencePoint) < Distance2D(rhs.target_pos, referencePoint);
		}
	};

	struct sort_by_cost {
		sort_by_cost(GameState* _state)
		{
			state = _state;
		}

		bool operator()(std::pair<Point3D, Units> lhs, std::pair<Point3D, Units> rhs)
		{
			return Util::GetUnitValues(lhs.second, &state->UnitInfo) < Util::GetUnitValues(rhs.second, &state->UnitInfo);
		}

		bool operator()(Units const lhs, Units const rhs)
		{
			return Util::GetUnitValues(lhs, &state->UnitInfo) < Util::GetUnitValues(rhs, &state->UnitInfo);
		}
	private:
		GameState* state;
	};

	struct sort_by_energy {

		bool operator()(Unit const* lhs, Unit const* rhs)
		{
			return lhs->energy > rhs->energy;
		}
	};


	struct sort_by_pathing_distance {
		Point3D referencePoint;
		QueryInterface* q;
		sort_by_pathing_distance(Point3D point, QueryInterface* query) {
			referencePoint = point;
			q = query;
		}

		bool operator()(Point3D const& lhs, Point3D const& rhs)
		{
			float l = q->PathingDistance(referencePoint, lhs);
			float r = q->PathingDistance(referencePoint, rhs);
			// If it's not pathable, use direct distance
			if (l == 0 || r == 0)
			{
				return Distance2D(lhs, referencePoint) < Distance2D(rhs, referencePoint);
			}
			else
			{
				return l < r;
			}
		}

		bool operator()(Unit const* lhs, Unit const* rhs)
		{
			return q->PathingDistance(referencePoint, lhs->pos) < q->PathingDistance(referencePoint, rhs->pos);
		}
	};

	struct sort_by_tag {
		bool operator()(Unit const* lhs, Unit const* rhs)
		{
			return lhs->tag < rhs->tag;
		}
	};

};