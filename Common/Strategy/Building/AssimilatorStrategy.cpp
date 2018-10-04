#include "AssimilatorStrategy.h"

using namespace sc2;


AssimilatorStrategy::AssimilatorStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
{

}


AssimilatorStrategy::~AssimilatorStrategy()
{
}



sc2::Point3D AssimilatorStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	foundUnit = nullptr;
	auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	Point3D buildPos;
	double distance = DBL_MAX;
	for (auto nexus : nexuses)
	{
		auto geysers = Util::FindNearbyUnits(Unit::Alliance::Neutral, IsVespeneGeyser(), nexus->pos, obs, 10);
		for (auto geyser : geysers)
		{
			if (query->Placement(this->buildingAction, geyser->pos))
			{
				auto dis = Distance3D(geyser->pos, nexus->pos);
				if (dis < distance && dis > 0)
				{
					buildPos = geyser->pos;
					distance = dis;
					foundUnit = geyser;
				}
			}
			else
			{
				debug->DebugSphereOut(geyser->pos, 3, Colors::Yellow);
			}
		}

		if (foundUnit) break;

	}

	return buildPos;
}