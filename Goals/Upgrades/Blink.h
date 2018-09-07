#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class BlinkGoal : public BaseAction
{
public:
	BlinkGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Twilight", 4, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::EFFECT_BLINK, 1));
		this->BaseAction::name = "Research Blink";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		auto upgrades = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::BLINKTECH);
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL), ABILITY_ID::RESEARCH_BLINK, obs))
			return 0;
		//if (state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 || state->ObservedUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0)
		{
			score = 6;
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::RESEARCH_BLINK, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, obs, actions);
	}
};