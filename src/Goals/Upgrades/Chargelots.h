#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class ChargelotGoal : public BaseAction
{
public:
	ChargelotGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Twilight", 2, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::EFFECT_CHARGE, 1));
		this->BaseAction::name = "Research Charge";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 1;
		auto upgrades = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::CHARGE);
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL), ABILITY_ID::RESEARCH_CHARGE, obs)) 
			return 0;
		int unitCount = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ZEALOT)).size();
		if (unitCount < 5) return 0;

		score = Util::ExponentialIncrease(unitCount, .25);

		if (state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANK] > 0 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED] > 0
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARAUDER] > 3)
		{
			score *= 3;
		}

		if (score > 10)
			return 9.91;

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUpgrade(ABILITY_ID::RESEARCH_CHARGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, obs, actions, query);
	}
};