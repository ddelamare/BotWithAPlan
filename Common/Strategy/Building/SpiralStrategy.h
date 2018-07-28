#pragma once
#include "BuildingStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
using namespace sc2;
class SpiralStrategy :
	public BuildingStrategy
{
public:
	SpiralStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon);
	~SpiralStrategy();
	sc2::Point3D virtual FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
};
