#pragma once
#include "Common\GameState.h"
#include "Common\Resource.h"
#include "Common/UnitFilters.h"
#include "Common/UnitHelpers.h"
class RushAnalyzer
{
public:
	double GetRushPossibiliy(const sc2::ObservationInterface* obs);
};

