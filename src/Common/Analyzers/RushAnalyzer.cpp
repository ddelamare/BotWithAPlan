#include "RushAnalyzer.h"
using namespace sc2;

double RushAnalyzer::GetRushPossibiliy(const sc2::ObservationInterface* obs)
{
	if (obs->GetGameLoop() > 8000) return 0;

	// Bio rush
	if (obs->GetUnits(Unit::Alliance::Enemy, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS)).size() >= 3)
	{
		return 10;
	}

	return 0;
}