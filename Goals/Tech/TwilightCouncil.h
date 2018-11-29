#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"

class TwilightCouncilGoal : public BaseAction
{
public:
	TwilightCouncilGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cyber", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->name = "Build Twilight";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state)  {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_TWILIGHTCOUNCIL, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, obs, actions, query, debug, state);
	}
};