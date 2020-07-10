#include "PylonStrategy.h"
#include "Common\Util\UnitHelpers.h"
#include "Common\Constants.h"
using namespace sc2;
PylonStrategy::PylonStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, Race race = Race::Protoss) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	unittype = UNIT_TYPEID::PROTOSS_PYLON;
	if (race == Race::Terran)
		unittype = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
	this->race = race;
	this->radius = 6;
}

PylonStrategy::PylonStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon, double radius, Race race = Race::Protoss) : BuildingStrategy(unit, needsClearance, needsPylon)
{
	unittype = UNIT_TYPEID::PROTOSS_PYLON;
	if (race == Race::Terran)
		unittype = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
	this->race = race;
	this->radius = radius;
}

PylonStrategy::~PylonStrategy()
{
}


sc2::Point3D PylonStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto nexii = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
	Point3D buildPos;
	if (nexii.size())
	{
		for (auto nex : nexii)
		{
			auto nearbyPylons = Util::FindNearbyUnits(Unit::Alliance::Self, IsUnit(unittype), nex->pos, obs, 10);
			if (nearbyPylons.size() == 0)
			{
				auto nearbyMinerals = Util::FindNearbyUnits(Unit::Alliance::Neutral, IsMineralField(), nex->pos, obs, 15);
				auto averagePoint = Util::GetAveragePoint(nearbyMinerals);
				auto vector = nex->pos - averagePoint;
				Normalize3D(vector);
				buildPos = nex->pos + (vector * (nex->radius + Constants::PYLON_RADIUS + 2));  // Add pylon and nexus radius
				debug->DebugSphereOut(buildPos, 3);
				return buildPos;
			}
		}
	}
	auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(unittype));

	if (pylons.size())
	{
		auto spiralizer = SpiralStrategy(buildingAction, useClearance, needsPower, radius, race);
		return spiralizer.FindPlacement(obs, actions, query, debug, state);
	}

 	return buildPos;
}