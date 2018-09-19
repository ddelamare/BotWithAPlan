#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Common/Util.h"
using namespace sc2;
class ExpandStrategy : public BuildingStrategy
{
public:
	ExpandStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
	{

	}


	~ExpandStrategy()
	{
	}

	sc2::Point3D GetFirstMatchingPlacement(Point3D startingPoint, Point3D direction,QueryInterface* query, DebugInterface* debug, int skipCount, int maxTries)
	{
		std::vector<QueryInterface::PlacementQuery> queries;
		bool foundSpot = false;
		Point3D buildPos;
		for (int i = 0; i < maxTries; i++)
		{
			auto offset = (i + skipCount) * direction;
			queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, startingPoint + offset));
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
		return buildPos;
	}

	sc2::Point3D FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		Point3D buildPos;

		int NEXUS_MAX_TRIES = 5;

		for (auto exp : state->ExpansionLocations)
		{
			auto furtherst = Util::FindFurthestInRadius(IsMineralField(), exp, obs, query, 15, Point3D());
			if (!furtherst) return buildPos;
			auto otherFurthest = Util::FindFurthestInRadius(IsMineralField(), exp, obs, query, 15, furtherst->pos);
			Point3D averagePoint;
			if (furtherst && otherFurthest)
			{
				averagePoint = (otherFurthest->pos + furtherst->pos) / 2;
			}
			else
			{
				return buildPos;
			}
			debug->DebugSphereOut(furtherst->pos, 2, Colors::Red);
			debug->DebugSphereOut(otherFurthest->pos, 2, Colors::Red);
			debug->DebugSphereOut(exp, 2, Colors::White);

			auto direction = averagePoint - exp;
			direction.z = 0;
			Normalize3D(direction);
			// Check for placement along the center vector
			buildPos = GetFirstMatchingPlacement(exp, direction, query, debug, 3, NEXUS_MAX_TRIES);

			if (buildPos.x != 0 || buildPos.y != 0)
				return buildPos;

			// To continue we try points on both sides of the initial vector

			//Find parallel vector a unit distance away and check along there
			//https://stackoverflow.com/a/4780141
			auto perpendicularVector = Point3D(-1 * direction.y, direction.x, 0);
			buildPos = GetFirstMatchingPlacement(exp + perpendicularVector, direction, query, debug, 3, NEXUS_MAX_TRIES);
			if (buildPos.x != 0 || buildPos.y != 0)
				return buildPos;
			buildPos = GetFirstMatchingPlacement(exp - perpendicularVector, direction, query, debug, 3, NEXUS_MAX_TRIES);
			
		}


		return buildPos;
	}
};

