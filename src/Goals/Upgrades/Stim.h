#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class StimGoal : public BaseAction
{
public:
	StimGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Techlab", 2, sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::EFFECT_STIM, 1));
		this->BaseAction::name = "Research Stim";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		double score = 0;
		auto upgrades = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::STIMPACK);
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::TERRAN_TECHLAB), ABILITY_ID::RESEARCH_STIMPACK, obs))
			return 0;
		int unitCount = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE)).size();
		if (unitCount < 5) return 0;
		score = Util::ExponentialIncrease(unitCount, .3);

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUpgrade(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, obs, actions, query);
	}
};