#include "PylonStrategy.h"
using namespace sc2;
PylonStrategy::PylonStrategy(ABILITY_ID unit, bool needsClearance, bool needsPylon) : BuildingStrategy(unit, needsClearance, needsPylon)
{

}


PylonStrategy::~PylonStrategy()
{
}


sc2::Point3D PylonStrategy::FindPlacement(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	float PYLON_RADIUS = 1.125;

	auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
	auto nexii = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	auto minerals = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField());
	Point3D buildPos;
	if (pylons.size())
	{
		//TODO: BUild next to nexuses without pylons
		auto spiralizer = SpiralStrategy(buildingAction, useClearance, needsPower, 8);
		return spiralizer.FindPlacement(obs, actions, query, debug, state);

	}
	else if(nexii.size())  // If there are no pylons, build next to nexus
	{
		auto mainNex = nexii[0];
		if (Distance2D(state->MineralDirection, Point2D(0, 0)) == 0) // Check if it's been set
		{
			//Calc mineral vector and normalize
			int visibleMinerals = 0;
			Point3D sum;
			for (auto min : minerals)
			{
				if (min->display_type == Unit::DisplayType::Visible)
				{
					sum += min->pos;
					visibleMinerals++;
				}
			}
			if (visibleMinerals > 0)
			{
				sum /= visibleMinerals;
				sum = sum - mainNex->pos;
				sum.z = 0;
				Normalize3D(sum);
				state->MineralDirection = sum;
			}
		}
		buildPos = mainNex->pos - (state->MineralDirection * (mainNex->radius + PYLON_RADIUS + 3));  // Add pylon and nexus radius
	}
 	return buildPos;
}