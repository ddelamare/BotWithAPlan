#include "BuildingStrategy.h"
using namespace sc2;
sc2::Point3D BuildingStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state) 
{
	auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
	Point3D buildPos = Point3D(0, 0, 0);
	Point3D offset = Point3D(5, 0, 0);
	if (state->MineralDirection.x > state->MineralDirection.y)
	{
		offset *= -1;
	}
	for (auto pylon : pylons)
	{
		if (query->Placement(this->buildingAction, pylon->pos + offset))
		{
			buildPos = pylon->pos + offset;;
			break;
		}
		else
		{
			debug->DebugSphereOut(pylon->pos + offset, 3, Colors::Yellow);
		}
	}
	return buildPos;
}

BuildingStrategy::BuildingStrategy(ABILITY_ID building, bool needsClearance, bool needsPylon)
{
	this->buildingAction = building;
	this->useClearance = needsClearance;
	this->needsPower = needsPylon;
}


BuildingStrategy::~BuildingStrategy()
{
}