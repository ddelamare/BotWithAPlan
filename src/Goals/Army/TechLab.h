#pragma once
#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/BuildingStrategy.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
using namespace sc2;
class TechLabGoal : public BaseAction
{
	vector<UNIT_TYPEID> addonTypes;
private:
public:
	TechLabGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Barracks", 4, sc2::UNIT_TYPEID::TERRAN_BARRACKS, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, 1));
		addonTypes.push_back(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
		addonTypes.push_back(UNIT_TYPEID::TERRAN_BARRACKSREACTOR);

		name = "Build Techlab";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		auto addons = obs->GetUnits(Unit::Alliance::Self, IsUnits(addonTypes)).size();
		auto barracks = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));

		return (addons < barracks.size()) * 6;

		return 0;
	}

	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool found = true;
		for (auto rax : obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS)))
		{
			if (!rax->add_on_tag)
			{
				actions->UnitCommand(rax, ABILITY_ID::BUILD_TECHLAB);
				found = true;
			}
		}
		return found;
	}

};