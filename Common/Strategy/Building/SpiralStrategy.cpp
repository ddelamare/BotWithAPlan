#include "SpiralStrategy.h"
using namespace sc2;
SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
{

}


SpiralStrategy::~SpiralStrategy()
{
}


sc2::Point3D SpiralStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
	Point3D buildPos;
	Point3D offset = Point3D(1, 0, 0);
	if (state->MineralDirection.x > state->MineralDirection.y)
	{
		offset *= -1;
	}
	int maxSpiralCount = 64;
	const int numDivisions = 8;
	const double radialDiff = 2 * M_PI / numDivisions;
	double xScalar = 0;
	bool foundSpot = false;
	for (int i = 0; i < maxSpiralCount; i++)
	{
		for (auto pylon : pylons)
		{
			if (query->Placement(this->buildingAction, pylon->pos + offset))
			{
				buildPos = pylon->pos + offset;
				foundSpot = true;
				break;
			}
			else
			{
				debug->DebugSphereOut(pylon->pos + offset, 3, Colors::Yellow);
			}
		}
		if (foundSpot) break;

		//Expand Spiral
		if (i % numDivisions == 0)
		{
			xScalar+= 1;
		}

		offset.x = sin(radialDiff * i) * xScalar;
		offset.y = cos(radialDiff * i) * xScalar;
	}



	return buildPos;
}