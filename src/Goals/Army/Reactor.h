#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/BuildingStrategy.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
using namespace sc2;
class ReactorGoal : public BaseAction
{
private:
public:
	ReactorGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Barracks", 4, sc2::UNIT_TYPEID::TERRAN_BARRACKS, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR, 1));
		name = "Build Reactor";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto reactors = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSREACTOR));
		auto barracks = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));

		if (reactors.size() == barracks.size())	return 0;
		int totalRax = reactors.size();
		auto nexus = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
		if (!totalRax)
		{
			return 5;
		}
		else if (totalRax >= ((2 * nexus.size())))
		{
			return 0;
		}
		else if (obs->GetFoodArmy() > 0)
		{
			return obs->GetFoodArmy() / (int)(3 * totalRax);
		}
		return 0;
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool found = true;
		for (auto rax : obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS)) )
		{
			if (!rax->add_on_tag)
			{
				actions->UnitCommand(rax, ABILITY_ID::BUILD_REACTOR);
				found = true;
			}
		}
		return found;
	}

};