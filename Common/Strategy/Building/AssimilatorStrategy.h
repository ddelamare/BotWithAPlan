#pragma once
#include "BuildingStrategy.h"

using namespace sc2;
class AssimilatorStrategy :
	public BuildingStrategy
{
public:
	AssimilatorStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon);
	~AssimilatorStrategy();
	sc2::Point3D virtual FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
};

