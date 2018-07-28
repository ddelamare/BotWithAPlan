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
		auto geysers = obs->GetUnits(Unit::Alliance::Neutral, IsUnit(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER));
		for (auto geyser : geysers)
		{
			if (query->Placement(this->buildingAction, geyser->pos))
			{
				auto dis = query->PathingDistance(geyser->pos, nexus->pos + Point3D(nexus->radius, 0, 0));
				if (dis < distance)
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