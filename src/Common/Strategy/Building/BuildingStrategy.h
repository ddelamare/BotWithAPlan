#pragma once
#include "sc2api\sc2_api.h"
#include "Common\GameState.h"
#include "Common\Util\Util.h"

class BuildingStrategy
{

public:
	const Unit * foundUnit;

protected:
	ABILITY_ID buildingAction;
	bool useClearance;
	bool needsPower;

public:
	sc2::Point3D virtual FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
	BuildingStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon);
	~BuildingStrategy();
};

