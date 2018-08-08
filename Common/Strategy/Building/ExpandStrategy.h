#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <Common/Util.h>
using namespace sc2;
class ExpandStrategy :	public BuildingStrategy
{
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

		// Find mineral patches clsoe to existing nexuses
		auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
		if (state->ExpansionLocations.size() == 0)
		{
			 // Get all minerals and sort by x , then y pos
			 auto minerals = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField());
			 int count = 0;
			 Point3D sum = Point3D();
			 const int MINERAL_DISTANCE_THRESHOLD = 150;
			 while (minerals.size() > 0)
			 {
				 Point3D origMineral = minerals[0]->pos;
				 sum = Point3D();
				 count = 0;
				 // Yay for O(N^2)!!!
				 for (int j = 0; j < minerals.size();)
				 {
					 auto cluster = std::vector<Point3D>();
					 auto dis = DistanceSquared3D(origMineral, minerals[j]->pos);
					 if (dis < MINERAL_DISTANCE_THRESHOLD)
					 {
						 sum += minerals[j]->pos;
						 count++;
						 // Erase this element
						 minerals.erase(minerals.begin() + j, minerals.begin() + j + 1);
					 }
					 else
					 {
						 j++;
					 }
				 }
				 
				 state->ExpansionLocations.push_back(sum / count);
			 }
		}
		else
		{
			for (auto exp : state->ExpansionLocations)
			{
				debug->DebugSphereOut(exp, 5, Colors::Red);
			}
		}

		return buildPos;
	}
};

