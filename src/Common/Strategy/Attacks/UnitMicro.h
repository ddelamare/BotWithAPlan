#pragma once
#include "Common/Util/Util.h"
#include <vector>
using namespace sc2;
class UnitMicro
{
public:
	UnitMicro() {}
	void virtual Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state) = 0;
};

