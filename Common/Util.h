#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
using namespace sc2;

const Unit* FindNearestResourceNeedingHarversters(const Unit* worker, const ObservationInterface* obs, QueryInterface* query)
{
	double distance = DBL_MAX;
	const Unit* target = 0;
	auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
	for (auto nexus : nexuses)
	{
		if (nexus->ideal_harvesters > nexus->assigned_harvesters)
		{
			auto dis = query->PathingDistance(worker->pos, nexus->pos+Point3D(nexus->radius,0,0));
			if (dis < distance)
			{
				distance = dis;
				target = nexus;
			}
		}

	}
	auto assimilators = obs->GetUnits(Unit::Alliance::Self, GetUnitsOfType(UNIT_TYPEID::PROTOSS_ASSIMILATOR));
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
		Units minerals = obs->GetUnits(Unit::Alliance::Neutral, GetUnitsOfType(UNIT_TYPEID::NEUTRAL_MINERALFIELD));
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