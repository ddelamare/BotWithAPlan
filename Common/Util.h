#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
using namespace sc2;

namespace Util {

	// Basically this function outputs a lower value as current percent increases over hallNeedingMiners percent and when percent is 0, returns score when zero
	const static double FeedbackFunction(double currentPercent,double targetPercent, double multiplier)
	{
		// equivalent to 1/((currentPercent / targetPercent) + (1/multiplier));
		return targetPercent / (currentPercent + (targetPercent / multiplier));
	}

	const static double ExponentialIncrease(double x, double dampener)
	{
		return x * x * dampener;
	}

	const static Unit* FindNearestResourceNeedingHarversters(const Unit* worker, const ObservationInterface* obs, QueryInterface* query)
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
		if (!hallNeedingMiners)
		{
			auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
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

	const static Unit* FindClosestAvailableWorker(Point3D point, const ObservationInterface* obs, QueryInterface* query, GameState* state)
	{
		auto units = obs->GetUnits(Unit::Alliance::Self, IsWorker());
		double distance = DBL_MAX;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			auto dis = query->PathingDistance(unit, point);
			if (dis < distance && !VectorHelpers::FoundInVector(state->ScoutingUnits, unit))
			{
				distance = dis;
				foundUnit = unit;
			}
		}
		return foundUnit;
	}

	const static Unit* FindClosetOfType(Filter filter, Point3D point, const ObservationInterface* obs, QueryInterface* query)
	{
		auto units = obs->GetUnits(Unit::Alliance::Self, filter);
		double distance = DBL_MAX;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			auto dis = query->PathingDistance(unit, point);
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

	const static Units FindNearbyUnits(Filter filter, Point3D point, const ObservationInterface* obs, double radius)
	{
		auto units = obs->GetUnits(filter);
		auto unitsNearby = Units();
		for (auto unit : units)
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
		auto units = obs->GetUnits(alliance,filter);
		auto unitsNearby = Units();
		for (auto unit : units)
		{
			auto dis = Distance3D(unit->pos, point);
			if (dis < radius)
			{
				unitsNearby.push_back(unit);
			}
		}
		return unitsNearby;
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

	std::string static GetStringFromRace(const sc2::Race & race)
	{
		switch (race)
		{
		case sc2::Race::Protoss: return "Protoss";
		case sc2::Race::Terran:  return "Terran";
		case sc2::Race::Zerg:	return "Zerg";
		default: return "Random";
		}
	}

};

struct Sorters
{
	struct sort_by_x {
		bool operator()(Point3D const & lhs, Point3D const & rhs)
		{
			return lhs.x < rhs.x;
		}
		bool operator()(const Unit* lhs, const Unit* rhs)
		{
			return lhs->pos.x < rhs->pos.x;
		}
	};
	struct sort_by_y {
		bool operator()(Point3D const & lhs, Point3D const & rhs)
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

		bool operator()(Point3D const & lhs, Point3D const & rhs)
		{
			return Distance2D(lhs, referencePoint) < Distance2D(rhs, referencePoint);
		}

		bool operator()(Unit const * lhs, Unit const * rhs)
		{
			return Distance2D(lhs->pos, referencePoint) < Distance2D(rhs->pos, referencePoint);
		}

		bool operator()(QueryInterface::PlacementQuery const lhs, QueryInterface::PlacementQuery const rhs)
		{
			return Distance2D(lhs.target_pos, referencePoint) < Distance2D(rhs.target_pos, referencePoint);
		}
	};

	struct sort_by_pathing_distance {
		Point3D referencePoint;
		QueryInterface* q;
		sort_by_pathing_distance(Point3D point, QueryInterface* query) {
			referencePoint = point;
			q = query;
		}

		bool operator()(Point3D const & lhs, Point3D const & rhs)
		{
			return q->PathingDistance(lhs, referencePoint) < q->PathingDistance(rhs, referencePoint);
		}

		bool operator()(Unit const * lhs, Unit const * rhs)
		{
			return q->PathingDistance(lhs->pos, referencePoint) < q->PathingDistance(rhs->pos, referencePoint);
		}
	};

};