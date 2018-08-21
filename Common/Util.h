#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
using namespace sc2;

namespace Util {

	const static Unit* FindNearestResourceNeedingHarversters(const Unit* worker, const ObservationInterface* obs, QueryInterface* query)
	{
		double distance = DBL_MAX;
		const Unit* target = 0;
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		for (auto nexus : nexuses)
		{
			if (nexus->ideal_harvesters > nexus->assigned_harvesters)
			{
				auto dis = query->PathingDistance(worker->pos, nexus->pos + Point3D(nexus->radius, 0, 0));
				if (dis < distance)
				{
					distance = dis;
					target = nexus;
				}
			}

		}
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
		for (auto assim : assimilators)
		{
			if (assim->ideal_harvesters > assim->assigned_harvesters)
			{
				auto dis = query->PathingDistance(worker->pos, assim->pos + Point3D(assim->radius, 0, 0));
				if (dis < distance)
				{
					distance = dis;
					target = assim;
				}
			}
		}

		if (target && target->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
		{
			auto nex = target;
			// Find nearby mineral patch.
			Units minerals = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField());
			float distance = std::numeric_limits<float>::max();
			for (const auto& m : minerals) {
				float d = DistanceSquared2D(m->pos, nex->pos);
				if (d < distance) {
					distance = d;
					target = m;
				}
			}
		}

		return target;
	}

	const static Unit* FindClosestAvailableWorker(Point3D point, const ObservationInterface* obs, QueryInterface* query, GameState* state)
	{
		auto units = obs->GetUnits(Unit::Alliance::Self, IsWorker());
		double distance = DBL_MAX;
		const Unit* foundUnit = nullptr;
		for (auto unit : units)
		{
			auto dis = query->PathingDistance(unit, point);
			if (dis < distance && !VectorHelpers::FoundInVector(state->ScoutingProbes, unit))
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
			auto dis = Distance3D(unit->pos, point);
			if (dis > distance && dis < radius)
			{
				distance = dis;
				foundUnit = unit;
			}
		}
		return foundUnit;
	}

	bool static IsAnyWorkerCommitted(ABILITY_ID ability, const ObservationInterface* obs)
	{
		auto workers = obs->GetUnits(Unit::Alliance::Self, IsWorker());
		for (auto worker : workers)
		{
			if (worker->orders.size())
			{
				for (auto order : worker->orders)
				{
					if (order.ability_id == ability) 
						return true;
				}
			}
		}
		return false;
	}

	bool static HasEnoughResources(int mineralsNeeded, int gasNeeded, const ObservationInterface* obs)
	{
		return obs->GetMinerals() >= mineralsNeeded && obs->GetVespene() >= gasNeeded;
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
		Point3D referencePoint;
		sort_by_distance(Point3D point) {
			referencePoint = point;
		}

		bool operator()(Point3D const & lhs, Point3D const & rhs)
		{
			return Distance3D(lhs, referencePoint) < Distance3D(rhs, referencePoint);
		}
	};

};