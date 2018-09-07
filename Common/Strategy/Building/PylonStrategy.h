#pragma once
#include "BuildingStrategy.h"
#include "SpiralStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Common/Util.h"
using namespace sc2;
class PylonStrategy :
	public BuildingStrategy
{
public:
	PylonStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon);
	~PylonStrategy();
	sc2::Point3D virtual FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
};

