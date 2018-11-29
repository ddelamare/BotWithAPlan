#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class TemplarArchivesGoal : public BaseAction
{
public:
	TemplarArchivesGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Twilight", 2, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, 1));
		this->name = "Build Archives";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_TEMPLARARCHIVE, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, obs, actions, query, debug, state);
	}
};