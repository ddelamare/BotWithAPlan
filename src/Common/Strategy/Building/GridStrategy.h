#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Common/Util/Util.h"
#include "Common\Constants.h"
using namespace sc2;
class GridStrategy: public BuildingStrategy
{
	float unitRadius = 1.0;
public:
	GridStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, float unit_radius) : BuildingStrategy(unit, needsClearance, needsPylon)
	{
		unitRadius = unit_radius;
		this->buildingAction = unit;
	}

	~GridStrategy()
	{
	}

	sc2::Point3D FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		// TODO: cache placements per step since it's not going to change till next obs
		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		Point3D buildPos;
		Point3D offset = Point3D(0, Constants::PYLON_RADIUS, 0);

		bool foundSpot = false;
		std::vector<QueryInterface::PlacementQuery> queries;
		auto spacing = Constants::PYLON_RADIUS + unitRadius;

		for (auto pylon : pylons)
		{
			queries.clear();
			
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.x += spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.y -= spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.y -= spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.x -= spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.x -= spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.y += spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));
			offset.y += spacing;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, pylon->pos + offset));

			// Batch do queries
			auto queryResults = query->Placement(queries);
			for (int i = 0; i < queryResults.size(); i++)
			{
				if (queryResults[i])
				{
					buildPos = Util::ToPoint3D(queries[i].target_pos);
					debug->DebugSphereOut(Util::ToPoint3D(queries[i].target_pos, 8), 3, Colors::Teal);
					foundSpot = true;
					break;
				}
				else
				{
					debug->DebugSphereOut(Util::ToPoint3D(queries[i].target_pos, 8), 3, Colors::White);
				}
				if (foundSpot) break;
			}
		}

		return buildPos;
	}
};

