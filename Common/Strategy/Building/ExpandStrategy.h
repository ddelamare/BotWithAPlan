#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Common/Util.h"
using namespace sc2;
class ExpandStrategy : public BuildingStrategy
{
	static int avgTries;
	static int numTries;
	static int sumTries;
public:
	ExpandStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
	{

	}


	~ExpandStrategy()
	{
	}


	sc2::Point3D FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		Point3D buildPos;

		int NEXUS_MAX_TRIES = 5;
		bool foundSpot = false;
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
			//debug->DebugSphereOut(furtherst->pos, 2, Colors::Red);
			//debug->DebugSphereOut(otherFurthest->pos, 2, Colors::Red);
			//debug->DebugSphereOut(exp, 2, Colors::White);

			auto direction = averagePoint - exp;
			direction.z = 0;
			Normalize3D(direction);
			for (int i = 0; i < NEXUS_MAX_TRIES; i++)
			{
				auto offset = (i+3) * direction;
				if (query->Placement(this->buildingAction, exp + offset))
				{
					buildPos = exp + offset;
					foundSpot = true;
					sumTries += i;
					numTries += 1;
					avgTries = sumTries / numTries;
					break;
				}
				else
				{
					//debug->DebugSphereOut(exp + offset, 5, Colors::Yellow);
				}
			}
			if (foundSpot) break;
		}

		return buildPos;
	}
};

int ExpandStrategy::avgTries = 0;
int ExpandStrategy::numTries = 0;
int ExpandStrategy::sumTries = 0;

