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
		// All buildings are placed at the exactly center of a square 
		auto roundedPoint = Point3D(round(startingPoint.x) + .5 , round(startingPoint.y) + .5 , 10);
		if (direction.x > 0) direction.x = 1;
		else if (direction.x < 0) direction.x = -1;
		if (direction.y > 0) direction.y = 1;
		else if (direction.y < 0) direction.y = -1;
		bool foundSpot = false;
		Point3D buildPos;
		bool isVertical = false;
		bool isHorizontal = false;
		if (direction.x == 0)
		{
			isVertical = true;
		}
		if (direction.y == 0)
		{
			isHorizontal = true;
		}

		if (query->Placement(this->buildingAction, startingPoint))
		{
			return startingPoint;
		}
		else
		{
			debug->DebugSphereOut((startingPoint), 4, Colors::Teal);
			debug->DebugTextOut(std::to_string(startingPoint.x) + " " + std::to_string(startingPoint.y), startingPoint);
		}

		for (int i = -maxTries; i < maxTries; i++)  
		{
			if (isHorizontal && (i * direction.x) < 0)
			{
				continue;
			}
			for (int j = -maxTries; j < maxTries; j++)
			{
				if (isVertical && (j * direction.y) < 0)
				{
					continue;
				}
				auto offset = Point3D(i, j, 0);
				debug->DebugSphereOut((roundedPoint + offset), 3, Colors::Yellow);
				queries.push_back(QueryInterface::PlacementQuery(this->buildingAction, roundedPoint + offset));
			}
		}
		std::sort(queries.begin(), queries.end(), Sorters::sort_by_distance(roundedPoint));
		// Batch do queries
		auto queryResults = query->Placement(queries);
		for (int i = 0; i < queryResults.size(); i++)
		{
			if (queryResults[i])
			{
				buildPos = Util::ToPoint3D(queries[i].target_pos);
				foundSpot = true;
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

		int NEXUS_MAX_TRIES = 7;

		for (auto exp : state->ExpansionLocations)
		{
			// If enemy is too close don't expand there
			if (Util::FindNearbyUnits(Unit::Alliance::Enemy, IsEnemy(), exp, obs, 15).size() 
				|| Util::FindNearbyUnits(Unit::Alliance::Self, IsTownHall(), exp, obs, 15).size())
			{
				continue;
			}
			auto furtherst = Util::FindFurthestInRadius(IsMineralField(), exp, obs, query, 15, Point3D());
			if (!furtherst) continue;
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
			buildPos = GetFirstMatchingPlacement(exp, direction, query, debug, 5, NEXUS_MAX_TRIES);

			if (buildPos.x != 0 || buildPos.y != 0)
				return buildPos;
		}


		return buildPos;
	}
};

