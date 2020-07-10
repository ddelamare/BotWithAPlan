#pragma once
#include "Common\GameState.h"
#include "Common\Resource.h"
#include "Common/Util/UnitFilters.h"
#include "Common/Util/UnitHelpers.h"
class RushAnalyzer
{
public:
	double GetRushPossibiliy(const sc2::ObservationInterface* obs);
};

