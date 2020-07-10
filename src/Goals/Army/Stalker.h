#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "Planner/Conditions/HaveGatewayTrainer.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class StalkerGoal : public BaseAction
{
public:
	StalkerGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Cybernetics", 4, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, 1));
		this->conditions.push_back(new HaveGateWayTrainer());
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STALKER, 1));
		this->BaseAction::name = "Build Stalker";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;

		auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());

		double percent = Util::GetUnitPercent(UNIT_TYPEID::PROTOSS_STALKER, 2, obs);
		score = Util::FeedbackFunction(percent, .4, 2.9);
		if (state->MaxEnemyUnits[UNIT_TYPEID::PROTOSS_VOIDRAY] >= 3
			|| state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_BANSHEE] >= 2
			|| (state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_MARINE] >= 1 && state->MaxEnemyUnits[UNIT_TYPEID::TERRAN_MARAUDER] <= 2))
		{
			score *= 4;
		}

		if (townhalls.size() == 1)
		{
			score /= 2;
		}

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryWarpUnit(UNIT_TYPEID::PROTOSS_STALKER, obs, actions, query, debug, state);
	}

};