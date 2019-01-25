#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
using namespace sc2;

class SpiralStrategy :
	public BuildingStrategy
{
	double _RadialDistance = 4;
private:
	UNIT_TYPEID unittype;
public:
	SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon);
	SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, double radialDistance, Race race);
	SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, Race race);
	SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, double radialDistance);
	~SpiralStrategy();
	sc2::Point3D virtual FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
};

