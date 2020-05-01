#include "SpiralStrategy.h"
using namespace sc2;
SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, double radialDistance, Race race) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	this->_RadialDistance = radialDistance;
	unittype = UNIT_TYPEID::PROTOSS_PYLON;
	if (race == Race::Terran)
		unittype = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;

}
SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, double radialDistance) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	this->_RadialDistance = radialDistance;
	unittype = UNIT_TYPEID::PROTOSS_PYLON;

}
SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
{
}

SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, Race race) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	unittype = UNIT_TYPEID::PROTOSS_PYLON;
	if (race == Race::Terran)
		unittype = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
}

SpiralStrategy::SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, Race race, bool useForwardPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	unittype = UNIT_TYPEID::PROTOSS_PYLON;
	this->useForwardPylon = useForwardPylon;
	if (race == Race::Terran)
		unittype = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
}

SpiralStrategy::~SpiralStrategy()
{
}


sc2::Point3D SpiralStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(unittype));
	Point3D buildPos;
	Point3D offset = Point3D(1, 0, 0);
	int rotationDir = 1;
	if (state->MineralDirection.x > state->MineralDirection.y)
	{
		offset *= -1;
		rotationDir = -1;
	}
	const int numDivisions = 4;
	int maxSpiralCount = numDivisions * 3;
	const double radialDiff = 2 * M_PI / numDivisions;
	double xScalar = 0;
	bool foundSpot = false;
	std::vector<QueryInterface::PlacementQuery> queries;

	for (auto pylon : pylons)
	{
		queries.clear();
		xScalar = 0;
		for (int i = 0; i < maxSpiralCount; i++)
		{
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));

			//Expand Spiral
			if (i % numDivisions == 0)
			{
				xScalar += 1;
			}

			offset.x = sin(rotationDir * radialDiff * i) * xScalar * this->_RadialDistance;
			offset.y = cos(rotationDir * radialDiff * i) * xScalar * this->_RadialDistance;
		}

		if (this->useForwardPylon) {
			std::sort(queries.begin(), queries.end(), Sorters::sort_by_distance(state->StartingLocation));
			std::reverse(queries.begin(), queries.end());
		}

		// Batch do queries
		auto queryResults = query->Placement(queries);

		for (int i = 0; i < queryResults.size(); i++)
		{
			if (queryResults[i])
			{
				buildPos = Util::ToPoint3D(queries[i].target_pos);
				foundSpot = true;
				break;
			}
			else
			{
				debug->DebugSphereOut(Util::ToPoint3D(queries[i].target_pos), 3, Colors::Yellow);
			}
			if (foundSpot) break;
		}
	}

	return buildPos;
}