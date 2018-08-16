#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>

class ChargelotGoal : public BaseAction
{
	bool hasUpgrade = false;
public:
	ChargelotGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Twilight", 4, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::EFFECT_CHARGE, 1));
		this->BaseAction::name = "Research Charge";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (!hasUpgrade && (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 || state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0))
		{
			score = 6;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeUpgrade = false;
		auto twilightCouncil = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL));
		for (auto tc : twilightCouncil) {
			//TODO: Check minerals spent by other goals
			if (tc->orders.size() == 0 && tc->build_progress == 1 && Util::HasEnoughResources(100,100, obs))
			{
				actions->UnitCommand(tc, ABILITY_ID::RESEARCH_CHARGE);
				madeUpgrade = true;
				hasUpgrade = true;
			}
		}

		return madeUpgrade;
	}
};