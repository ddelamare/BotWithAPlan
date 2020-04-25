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

		int cannons = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PHOTONCANNON)).size();
		auto townhalls = obs->GetUnits(IsTownHall()).size();

		if (townhalls == 0)
			return 0;

		auto percent = (double)cannons / (townhalls * 3); // 3 cannons per nex should be fine

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