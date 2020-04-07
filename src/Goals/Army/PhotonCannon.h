#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Strategy/Building/SpiralStrategy.h"
#include "Common/Util.h"

class PhotonCannonGoal : public BaseAction
{
public:
	PhotonCannonGoal() : BaseAction()
	{
		this->conditions.push_back(new BaseCondition("Build Forge", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_FORGE, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON, 1));
		this->BaseAction::name = "Build Photon Cannon";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto score = 0.0;

		int unitFood = 2 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)).size();
		auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent zealots
		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_REAPER] >= 2
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_BANSHEE] >= 2)	
		{
			score = Util::FeedbackFunction(percent, .4, 2);
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_PHOTONCANNON, UNIT_TYPEID::PROTOSS_PHOTONCANNON , obs, actions, query, debug, state);
	}
};